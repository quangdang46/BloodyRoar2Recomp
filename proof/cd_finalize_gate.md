# CD finalize gate — wedge diagnosis (port 8999)

Fresh headless instance `:8999` (`PSX_DMAPROBE=1 PSX_CDEV=1 PSX_CDEVDBG=1`),
navigated with `/tmp/nav_arcade_8999.py` (port-adjusted copy of the 8995
script). Wedge reproduced deterministically: last CD command `Pause(0x09)`
exec'd at **frame 3866**, `sector_read_pos` frozen `2060/2340`,
`pending.cmd=0x09` residue, `ring_dropped=2`, `ring_partial=1824`,
`B0/0B TestEvent` frozen at **1,462,829** (blocked, not spinning — same
count as the 8995 capture). Instance killed after capture.

## VERDICT: (A) is literally TRUE — descriptors ARE corrupted — but the
## initiating defect is producer starvation, not the overwrite itself.

Both halves are proven below. (C) is ruled out: device delivery is
byte/timing-exact (consecutive LBAs, `irq_present_max_late=0`, last sector
lba 8801 delivered).

---

## 1. Live wedge dumps (frame ~159k, still advancing)

Channel ring cells (`0x801fc680` block):

| cell | value | meaning |
|---|---|---|
| `0x801fc680` | `0x00002711` | seq-B expected (SM `-0x3980`) |
| `0x801fc684` | `0x0002` (u16) | seq-A counter (SM `-0x397c`) |
| `0x801fc688` | `1` | async-DMA-mode flag (`cd_read_params_store` a0&1) |
| `0x801fc68c` | `0` | **SM repump request — CLEAR** |
| `0x801fc690/94/98` | `0 / 1 / 0` | SM scratch |
| `0x801fc69c` | `0` | finalize-pending flag (SM `-0x3964`) |
| `0x801fc6a0` | `0` | SM error/retry counter |
| `0x801fc6a4` | `2` | **submit/produce cursor** |
| `0x801fc6a8` | `0` | **finalize cursor** |
| `0x801fc6ac` | `0x1e` (30) | **consume/poll cursor — parked** |
| `0x801fc6b0` | `0` | mode flag → **sync path** (no async staging buf) |
| `0x801fc6b4` | `0xffffffff` | stuck-entry reset flag (`cd_read_param_block` a2) |
| `0x801fc6b8` | `0` | param flag |
| `0x801fc6bc` | `0x80010be0` | next-entry data ptr (table+0x400+1*0x7e0) |
| `0x801fc6c0` | `0x80010000` | **entry-table base** (`cd_dir_table_set` arg) |
| `0x801fc6c4` | `0x20` | capacity = 32 entries |
| `0x801fc6c8/cc` | `2 / 0` | mirror cursors (same state) |
| `0x801bcc88` | `0x0000000a` | **SM status = 10 = last run SUCCEEDED** |
| `0x801feb18` | `0x80010020` | cached entry ptr = table+0x20 (idx 1) |
| `0x801feb10/14` | `0 / 0` | finalize + repump callbacks — none |
| `0x801bc848/4c` | `0 / 0` | **armed session callbacks — BOTH EMPTY** |
| `0x801bcba8/ac` | `0 / 0` | saved session callbacks — also empty |
| `0x801bdee8` | `0` | → `cd_dma_chan_reset` would take direct `DeliverEvent(0xF0000009,0x20)` |
| `0x801bde38` | `0xf1000008` | TestEvent spec the waiter polls |
| `0x801be768` | `0xffffffff` | job counter sign-set (chan_job_pump skips) |
| `0x801f4738/3c` | `1 / 1` | tile_stream_pump outer gate ARMED |

## 2. The entry descriptors are corrupted — verdict (A) evidence

Table base `*(u32*)0x801fc6c0 = 0x80010000`, 32 slots × `0x20`. Dump of
the first 8 slots + the consumer's parked slot 30:

```
entry 0 @80010000: 1d000000 ffffffff ffffffff ...   u16@+0=0x001d  +4=+6=0xffff
entry 1 @80010020: ffff ffff ... (all 0xFF)         u16@+0=0xffff  +4=+6=0xffff
entry 2 @80010040: ffffffff 00000000 2cc20000 8458... u16@+0=0xffff
entry 3 @80010060: 344f0500 60110600 8cd30600 ...   looks like (tag,val) index records
entry 4 @80010080: 20c20000 10000000 09000000 ...
entry 5 @800100a0: 03000100 04002100 23000500 ...   incrementing id/offset pairs
entry 6 @800100c0: 64084600 0b00630c ...
entry 7 @800100e0: 6900a514 1100870c ...
entry30@800103c0: 0a050000 0000...                   u16@+0=0x050a — consumer's target
```

This is **file/stream payload** (structured index records), not channel
entries. A valid in-flight entry must have `u16@+0 == 0x160` (tag check at
`0x801997a4`), then `u16@+0 ∈ {0,1,2,3,4}` as lifecycle state
(`cd_channel_poll`/`cd_dir_entry_finalize`). **Every slot has a nonzero
garbage `u16@+0`** → the state machine exits at its very first gate
(`0x80199528 lhu (entry); bnez → status 4`) and can never reach the
`u16@+6 - 1 == u16@+4` finalize gate at `0x80199bc4-0x80199bd0`.

Concrete `+4/+6` mismatch on the cached entry (`0x80010020`, all `0xFF`):
`u16@+6-1 = 0xfffe != u16@+4 = 0xffff` → gate FAIL even if earlier checks
passed.

## 3. Who overwrote them — the §8 arena-sharing recurrence, on the
##    FILE-LOAD/STREAM path this time

`dmaprobe` channel-3 (CDROM DMA) kicks, two distinct populations:

- **SM entry writes** — `pc=0x80199694` (inside `cd_mc_state_machine`,
  the `jal cd_chan_cmd_submit` site), `b=0x00000008` (8 words = one
  `0x20` entry), `0x20`-stride addrs `0x80010000→0x800103E0`, cycling.
  Healthy production through **frame 3442** (557 such kicks total).
- **Stream payload writes** — `pc=0x801978AC` (`cd_cmd_issue_b`),
  `b=0x00010200`, sequential `0x800`-stride blocks starting at
  `0x80010000`: **f3464, f3691, f3705, f3755** (and earlier periodic
  restarts). Each covers the whole `0x400`-byte descriptor table.

Last table overwrite: **f3755** (`0x80010000`, 0x800 bytes →
`0x80010000-0x800107FF`). Wedge Pause: **f3866** — 111 frames later.
ch4 file-data kicks (`pc=0x8019BBAC cd_reg_window_op`, src inside the
arena `0x80010C20`/`0x80011220`) stop after **f3754** — matching the
prior proof's "zero ch4 after ~f3696".

So: `cd_dir_table_set(0x80010000, 0x20)` (via `tile_stream_open`,
`menu.c:2258`, `_DAT_801f4754 = scene_entry param_1 = 0x80010000`,
`game.c:1619/1639`) puts the live descriptor ring in the same scratch
arena the stream/file DMA fills. Same §8 collision class as the FMV bug —
this time on the `menu_res_load_sm`/stream path.

## 4. BUT the producer was already dead BEFORE the overwrite — the
##    initiating defect is (B): no driver invokes cd_mc_state_machine

Timeline proves ordering:

- Last SM entry-write DMA: **f3442**.
- First stream overwrite of the table: **f3464** — *22 frames later*.
- SM status cell `0x801BCC88 = 0x0a`: status 10 is written only on the
  SM's **success path** (`0x80199b10`, mid-function before the issue-read
  tail). Any post-corruption invocation would exit via status 3/4/5/6/7/8/9
  — the cell still reads `0x0a` → **the last SM run succeeded and no SM
  run has happened since ~f3442** (a silent `v0==5` early-out at
  `0x801994d0` can't be fully excluded, but zero entry-write DMAs in 424
  frames says it isn't reaching the issue path regardless).
- `fntrace_arm` on `0x80199440 / 0x80198FE4 / 0x801990F0 / 0x80199360`
  for 6 s at the wedge: **0 hits on all four** (consumer `cd_channel_poll`
  also not running — the load thread is suspended, not polling).
- `0x801fc68c = 0`: the per-frame driver (`tile_stream_pump` `menu.c:2277`
  / `hp_bar_tick` `menu.c:11552`, outer gate `0x801f4738=1` armed) never
  fires. In **sync mode** (`0x801fc6b0=0`) the SM cannot re-arm `fc68c`
  itself (that store at `0x8019949c` is gated on the async path), and
  `cd_dir_reset`/`hp_bar_tick` only clear it — so the per-frame path
  cannot sustain production.
- The sustained driver is therefore the **IRQ callback path**:
  `cd_response_pump` (generated `0x801979F8`, healthy — fires every CD
  IRQ) invokes `*(0x801bc84c)` on response-bit-4 and `*(0x801bc848)` on
  bit-2 (`0x80197A48-0x80197A9C`). **Both cells are 0** → nothing drives
  the channel/SM layer per sector.

`cd_read_setup` (`0x80198c90`, delegated) is what installs the
INT2/step session callbacks via `mc_session`/`mc_session_b`
(`0x80195d64/0x80195d78`) from the caller's channel struct
(`0x80198D44-0x80198DAC`). At the wedge neither the armed cells nor the
saved cells (`0x801bcba8/ac`) hold anything — the session callbacks are
gone while the consumer is still parked on entry idx 30.

## 5. Exact gate conditions decoded (generated `0x80199440`,
##    `psxrecomp/docs/internal/cdmc_80199440.asm` + `full_46.c:11589`)

Entry ptr = `*(u32*)0x801fc6c0 + (*(u32*)0x801fc6a4 << 5)`, cached at
`0x801FEB18`. Per invocation the SM:

1. `*0x801fc69c==1` → return (finalize busy).
2. `jal 0x80195d44` response probe; `==5` → silent return; resp byte
   `&4` → status 3 return.
3. `lhu entry+0 != 0` → **status 4 return** ← *fails here on every slot*
   (all `u16@+0` are payload garbage).
4. Issue 8-word template DMA/copy into entry, write cmd words
   `0x20943/0x1323` to the `0x801CCC40-54` register window, store resp
   u32 → `entry+0x1c`.
5. Tag checks: `u16@+0==0x160`, `(u16@+2>>10)&0x1f == *0x801fc698`,
   `lh *0x801fc684 == lhu entry+4`, `*0x801fc680==0 || entry+8==*0x801fc680`
   — any miss → status 5/6, entry cleared.
6. Progress check `*0x801fc6c4 - *0x801fc6a4 - 1 < u16 entry+6` →
   in-progress mark `entry+0=1`, status 8, return.
7. Tail `0x80199b10`: status `0xa`, seq counters bumped, next-data ptr
   `0x801fc6bc = table + cap*0x20 + submit*0x7e0`.
8. **Finalize gate `0x80199bc4`: `u16 entry+6 - 1 == u16 entry+4`** →
   set `*0x801fc69c=1`, issue the `0x1f8` read, `entry+0=3`,
   `submit++`, then `jal 0x801990f0 cd_dir_entry_finalize` →
   `entry+0=2` → consumer `cd_channel_poll` sees state 2 → ch4 submit →
   `cd_dma_chan_reset` → `DeliverEvent(0xF0000009,0x20)` →
   `TestEvent(0xF1000008)`.
   Gate miss → prefetch path instead (no finalize).

## 6. What SHOULD have happened / recommended fix location

On hardware the same guest DMAs hit the same addresses — the ring only
works because the producer is driven **per CD IRQ** through the armed
session callbacks and keeps entries cycling fast enough that the stream
plow-through doesn't strand the consumer. Here the producer has had **no
driver since ~f3442**: `0x801bc848/0x801bc84c == 0` and `0x801fc68c == 0`.

Recommended investigation order:

1. **`game/cd.c` session-callback lifecycle** — find what cleared
   `0x801bc848/0x801bc84c` (candidates: `cd_chan_reset_b` `0x8019746c`,
   `cd_subsystem_init` `0x801974b8`, `cd_read_setup` timeout/restore
   path in generated `0x80198c90`, or `mc_session(_b)` restore). If the
   clear ran while the channel consumer was still parked, that is the
   faithful-behavior break — the IRQ callback that drives
   `cd_mc_state_machine` per sector is missing. Check whether
   `cd_read_setup`'s generated body completed its install for THIS load
   (the `0x80198D50/0x80198DAC` `mc_session` calls) or whether a
   delegated boundary (`cd_read_setup`/`cd_read_open`/`cd_cmd_issue`)
   dropped the install.
2. **Arena sharing (secondary, §8 recurrence)** — even with the driver
   restored, the stream DMA will keep overwriting descriptors whenever
   the producer falls behind. If (1) shows callbacks were correctly
   armed on HW-equivalent timing, the remaining divergence is timing:
   the runtime must present CD IRQs / deliver ch4 completions at the
   cadence that lets the guest keep the ring ahead of the `0x800`-stride
   stream writes. Fix location would be `psxrecomp/runtime/src/cdrom.c`
   sector/IRQ cadence or `dma.c` ch4 completion — NOT the gate itself.
3. `cd_mc_state_machine` itself needs **no change** — delegated to
   generated is correct; its gates are faithful.

## 7. Reproduction

```
PSX_DMAPROBE=1 PSX_CDEV=1 PSX_CDEVDBG=1 \
  ./build/BloodyRoar2_Recompiled --disc ".../Game.cue" \
  --debug-port 8999 --headless
# /tmp/nav_arcade_8999.py ; wedge ~frame 3866
read_ram 0x801fc680 0x60 ; read_ram 0x80010000 0x400 ; read_ram 0x801bc848 0x10
fntrace_arm 0x80199440/0x80198FE4/0x801990F0/0x80199360 → 0 hits/6s
bioscall_dump → B0/0B frozen 1462829
```
