# MARK-mode event wedge — root cause analysis

Date: session continuing `proof/mark_event_delivery_failure.md`.
Live instance: debug port 9400, headless arcade run, wedged at frame ~77k.

## Verdict

**The MARK-event delivery machinery is faithful. The wedge is NOT a
`deliverEvent`/`testEvent`/`waitEvent` defect.** The event
`DeliverEvent(0xF0000009, 0x20)` is only *produced* by CD channel-4 DMA
completion (`cd_dma_chan_reset`, game/cd.c:5146). In the wedge the CD
streaming read froze upstream in the `psxrecomp/runtime/src/cdrom.c`
model, so the producer never runs again and the game's kernel
`waitEvent(0xF1000008)` spin never sees `PENDING`. This is the same
signature already documented as the CD deferred-coroutine starvation in
`proof/arcade_spawn_phase_starvation.md` (TCB `0x00008FE0` parked in the
per-frame `cd_timer_read → kernel yield → cd_chan_full_init` loop).

Classification: **runtime/hardware-model defect in the CD-ROM path** —
not a recompiler bug, not a faithful-OpenBIOS semantic mismatch, and not
IRQ/coroutine starvation of the event *delivery* path itself (delivery
works; the producer is gone).

## 1. OpenBIOS event services — verified faithful

Recompiled OpenBIOS (`psxrecomp/generated/OpenBIOS_full.c`,
`deliver_event_ret=0`, call-HLE off) implements the documented kernel
semantics:

- `deliverEvent` RAM `0x2610` / ROM `0xBFC205E4`: loads table ptr from
  `mem[0x120]`, count from `mem[0x124]`, stride `0x1C`; per slot checks
  `flags & 0x2000` (ENABLED), exact `class`/`spec` match; MARK mode
  `0x2000` stores `0x4000` (PENDING) into `flags`; CALLBACK mode
  `0x1000` invokes `handler`. Matches PCSX-Redux OpenBIOS
  `EventInfo {class, flags, spec, mode, handler, u1, u2}` (7 words).
- `testEvent` `0xBFC0574C`: PENDING → restore ENABLED, return 1;
  ENABLED → return 0. `waitEvent` `0xBFC0578C`: PENDING → consume+return;
  ENABLED → busy-spin on `flags` until PENDING (verified in generated
  code, lines ~78737+: `beq flags,0x4000 → consume`, `bne flags,0x2000 →
  ret 0`, else loop).
- `openEvent` `0xBFC05648` returns `slot | 0xF1000000`;
  `enableEvent`/`disableEvent`/`closeEvent` decode `handle & 0xFFFF`
  as the slot index — so `0xF1000008` → slot 8.

B0 dispatch: syscall vector `0xB0` → handler RAM `0x2AA0` → jump table
at RAM `0x49CC`. Verified live (`mem_words 0x49CC`):
index 7→`0x2610` DeliverEvent, 8→openEvent, 9→closeEvent,
10→waitEvent, 11→testEvent, 12→enableEvent, 13→disableEvent,
14–16→`0xBFC0688C/8F8/924`, 23(0x17)→`0x29CC` (kernel thread/exception
context restore — the per-frame `bios_svc_trace` storm source).

## 2. Event storage — verified live, not assumed

`evcb_snapshot` (port 9400):

- table base `0x8E08` (from `mem[0x120]`), 16 entries × `0x1C`.
- Slot 8 (`0x8EE8`): class `0xF0000009`, status `0x2000` (ENABLED),
  spec `0x00000020`, mode `0x00002000` (MARK), handler 0 — exactly the
  event `cd_ext_init_once` opened via `OpenEvent(0xF0000009,0x20,0x2000)`.
- Slots 0–7 are CALLBACK (`0x1000`) events (class `0xF4000001`/`0xF0000011`)
  and work; slots 9–15 FREE. Only slot 8 is MARK.

`wtrace` armed over `0x8EE8..0x8F04` during the wedge: **zero writes** —
`deliverEvent` never reached the MARK store for slot 8 because it was
never *called* with `(0xF0000009,0x20)` after the CD froze.

## 3. Delivery trace — who produces the event

`hle_dump`/`bioscall_dump` timeline for handle `0xF1000008`:

| seq | call | last cycle | note |
|-----|------|-----------|------|
| 2132 | B0/08 OpenEvent(0xF0000009,0x20,0x2000) | 215.7M | → handle 0xF1000008 |
| 2133 | B0/0C EnableEvent | 215.7M | |
| 2136/2141 | B0/0A WaitEvent / B0/0B TestEvent ra=0x8019CF48/0x8019D550 | ~236M | early loads OK |
| 2429/2431 | WaitEvent/TestEvent again | ~984–987M | final wedge window |
| 2430 | **B0/07 DeliverEvent(0xF0000009,0x20) ra=0x8019B88C** | **987,475,893** | LAST delivery ever |

After cycle ~987M (frame ~1569): **no further DeliverEvent, TestEvent,
WaitEvent, or any CD command**. `cdrom_command_history` last entry
seq144 = cmd `0x09` at cycle 1,062,986,348 / frame 1569 — then silence
for ~46 G cycles / ~68 K frames. `cdrom_state` frozen:
`pending{cmd:0x09,active:0,phase:1}`, `reading:0`, `stat:0x02`,
`request:0x80` (BFRD stuck set), `sector_read_pos:2060/2340`,
`bfrd_set:2047 vs bfrd_clear:13`, `ring_dropped:1`,
`ring_partial:898`, `irq_present_late:1519`.

`pc_probe` on waitEvent spin blocks (`0xBFC057B8/C0/E0`) and testEvent:
**0 hits** — the game thread is not spinning in the event wait; it is
parked in the CD deferred-interrupt coroutine (see §5).

## 4. Why `deliverEvent` "ran but never marked READY"

Earlier evidence ("DeliverEvent fires repeatedly yet slot8 stays
0x2000") is explained by timing, not by a delivery bug:

- `deliverEvent` at `0x2610` *does* execute (~278 hits/3 s probe), but
  those calls carry IRQ-side class/spec pairs (e.g. `0xF0000003`) that
  match no ENABLED slot — probe of the match-path blocks (`0x2690+`)
  recorded **0 hits**.
- The only `DeliverEvent(0xF0000009,0x20)` calls come from
  `cd_dma_chan_reset` on ch4 DMA completion. The last one fired at
  cycle 987,475,893 — coincident with the last TestEvent — i.e. the
  final sector's completion *was* delivered; the load simply needed
  more sectors that never arrived because the CD model froze mid-stream
  (`sector_read_pos` stuck at 2060/2340, `reading:0`, no new commands).
- A diagnostic `write_ram` of `0x4000` into slot8 flags during this
  session was consumed back to `0x2000` (wait/test side works), but
  nothing re-polls the event afterwards — the consumer is the parked
  coroutine, not a spinning `TestEvent`.

## 5. Where the game thread actually is

`sp_ring` tail — identical 3-call loop every frame, TCB `0x00008FE0`:

```
target 0x8019A53C  cd_timer_read      (game/cd.c, generated-delegated)
target 0x00002924  kernel yield tramp
target 0x8019A87C  cd_chan_full_init   (game/cd.c, generated-delegated)
```

~640 k cycles/iteration, `frame` advancing — the deferred CD coroutine
retries its wait once per frame and is never resumed because the
read-complete doorbell never fires. VBlank IRQ keeps dispatching
(`undeliverEvent`×5 → `deliverEvent` → B0 svc churn in `dispatch_tail`),
so the machine looks alive while the battle FSM (`DAT1==3`, phases
`+0xb4/+0xb6==0x0300`, `+0x92==0x0100`) is frozen downstream — exactly
the state catalogued in `proof/arcade_spawn_phase_starvation.md`.

## 6. Root cause and remaining unknown

**Root cause (proximate):** `cdrom.c` stopped servicing the battle-asset
streaming read at frame ~1569. Sector ring partially consumed
(2060/2340), BFRD re-asserted without matching clears
(`bfrd_set:2047` vs `bfrd_clear:13`), ring drops/partials recorded, and
`irq_present_late:1519` — the read-complete INT the coroutine waits on
is lost in the ring-flow-control / IRQ-present scheduling path
(`s_ring_*`, `s_bfrd_*`, `irq_present_*`, `pending_arm` — suspects
already listed in `arcade_spawn_phase_starvation.md` §"Suspect area").

**Narrowest remaining unknown:** the exact `cdrom.c` line where the
streaming read's completion is dropped — ring-slot overwrite vs BFRD
re-arm vs `irq_present` deadline miss. This needs a focused
cdrom.c investigation (the `cdfn_dump`/`cdrom_trace` instrumentation
already in the tree supports it); it is upstream of and independent of
the event layer.

**No fix implemented this session:** the faithful fix belongs in
`psxrecomp/runtime/src/cdrom.c` ring/BFRD/IRQ-present flow control, and
the precise defect line is not yet proven — a patch now would be
speculative (forbidden by the no-guess rule). No `game/*.c` change is
warranted; forcing the event flag or phase words would be fake delivery.

## 7. What was ruled out (exhaustive)

- Recompiler miscompile of `deliverEvent`/`testEvent`/`waitEvent` —
  generated code matches OpenBIOS source semantics instruction-for-
  instruction (loop stride `0x1C`, flag gates `0x2000`/`0x4000`, MARK
  store to `flags`).
- Wrong event-table layout or handle decode — live memory matches
  OpenBIOS `EventInfo`; `0xF1000008 & 0xFFFF = 8` → slot `0x8EE8`.
- B0 dispatch mis-mapping — table at `0x49CC` verified; calls reach the
  right ROM functions.
- IRQ/coroutine starvation *of delivery* — `deliverEvent` executes;
  CALLBACK events (slots 0–7) fire their handlers; VBlank/IRQ dispatch
  is healthy.
- Game-side event misuse — game opened/enabled/tested/waited on the
  correct class/spec/mode; producer contract (`cd_dma_chan_reset` →
  `DeliverEvent(0xF0000009,0x20)`) is intact in `game/cd.c:5146-5251`.
