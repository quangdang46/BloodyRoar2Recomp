# Who sets 0x801FC68C — the SM self-arms; the drivers went silent, not the flag

Date: static audit + live capture on fresh instance **debug port 8985**
(same wedge reproduced: `gs=8 ph=03`, producer `0x801fc6a4`=2,
finalize `0x801fc6a8`=0, fc68c=0, bc848=bc84c=0). Wedged reference
instance 8973 left running for follow-up.

## 1. Writer table — exhaustive

Every access to guest `0x801FC68C` in `generated/` (base `lui 0x8020` +
imm `-14708`, or `lui 0x801F`-family equivalents — all forms checked):

| PC | Function | Instr | Effect |
|---|---|---|---|
| 0x80159880 | tile_stream_pump 0x80159864 | `lw $v0,-14708($s0)` | reads flag (gate) |
| 0x80159898 | tile_stream_pump | `sw $zero,-14708($s0)` | **clears after SM returns** |
| 0x8019904C | cd_dir_reset 0x80199010 | `sw $zero,0xC68C($at)` | clear (clean `cd_dir_reset` mirrors it, game/cd.c:1569) |
| menu.c:2279 / 11554 | tile_stream_pump / hp_bar_tick | `MW32(0x801fc68c,0)` | clean clears after `cd_mc_state_machine()` |
| **0x8019949C** | **cd_mc_state_machine 0x80199440** | **`sw $a0,-14708($at)`** | **the ONLY nonzero writer in the binary** |

No pointer-indirection writer exists: the fc6xx block is always reached
via the `0x80200000`-base negative-offset idiom; a runtime `wtrace_range`
on the cell (below) recorded **only** pc=`0x8019949C` writes.

## 2. What the SM stores — constant 1, not the caller's a0

`cd_mc_state_machine` prologue sets `$a0 = 1` at **0x8019944C**
(`li $a0,1`, the `fc69c==1` reentrancy compare) *before* any use of the
incoming argument. The store at 0x8019949C therefore writes **1**
unconditionally on its path — the IRQ callback's a0
(`lbu @0x801BCB25`) is irrelevant. The clean delegation
`br2_call_generated(0x80199440,0,0,0,0)` (game/cd.c:3435) is faithful
w.r.t. arguments; dropping a0 is a non-bug.

Gate to reach block_8019948C (the store's block), in order
(generated/SLES_017.22_full_46.c ~11668+):

1. `*(0x801FC69C) != 1` — reentrancy guard.
2. `*(0x801FC688) != 0`.
3. `*(*(0x801BCC60)) & 0x01000000 != 0` — `0x801BCC60` holds
   `0x1F801098` (DMA channel-1 register file; plan doc identifies it as
   the MDEC-out channel status). **Bit 0x1000000 = channel busy.**
4. Then `sw 1 -> 0x801FC68C` (delay slot of the `fc6b0` branch), and
   `fc6b0==0 -> re-arm`; `fc6b0!=0 -> fc6a0++` (error-retry counter).

So fc68c is a **self re-arm**: each SM run that observes the DMA
channel busy re-arms the per-frame pump for one more run. It is not
"re-arm if entries remain" — it is "keep pumping while the channel is
busy". Once an SM run sees the channel idle it takes the
`0x801994C4 -> cd_result_wait_b(5, sp+0x30)` path and returns without
re-arming; the per-frame callers then clear fc68c and the flag is
dead until the SM is invoked again by another driver.

## 3. SM invocation paths (all three went silent)

| Path | Mechanism | Evidence |
|---|---|---|
| IRQ callback | `cd_response_pump` (0x801979F8) jalr's `*(0x801BC84C)` at 0x80197A64 → ra 0x80197A6C. bc84c = `cd_mc_sm_wrap` 0x80198FE4, installed by `cd_mode_set_0e` 0x80198F60 when `(mode&0x100)!=0` (caller passes 0x1E0; menu.c:2375). Parked/restored around file reads by `cd_read_setup`/`cd_read_open` (bc84c↔bcbac). | fntrace: 0x80198FE4 entries ra=0x80197A6C, a0=1, a1=0x801FC6D0 |
| DMA channel dispatcher | `br2eu_func_8019AF7C` walks the 7-slot handler table @0x801BDDF4 driven by mask `*(*(0x801BDDF0)=0x1F8010F4) & 0x7F`; jalr at 0x8019B01C → ra 0x8019B024. SM is a table entry. | fntrace: 0x80199440 entries ra=0x8019B024 |
| Per-frame pump | tile_stream_pump 0x80159864 / hp_bar_tick gate on `0x801F4738!=0 && 0x801FC68C!=0`, run SM, then clear fc68c. | menu.c:2277, 11552 |

## 4. Runtime proof — the flag DID fire; the drivers stopped

Fresh run on :8985, probes armed before nav
(`wtrace_range 0x801FC68C-90`, `wtrace_add` bc848/bc84c/fc688-94,
`fntrace_arm` on 0x80199440/0x80198FE4/0x801979F8/0x80198F60/0x80198590/
0x80198908/0x80198C90/0x801985C4):

- **wtrace fc68c**: 78 writes, ALL `pc=0x8019949C`, ALL `new=0x00000001`,
  ALL `ra=0x80197A6C` (IRQ-callback path). First frame 5614, **last
  frame 5703**. No other PC ever wrote the cell.
- **fntrace**: last `0x80199440` (SM) entries at frames **5703-5704**
  (ra 0x80197A6C IRQ + 0x8019B024 dispatcher); last `0x80198FE4`
  (sm_wrap) at frame 5704. **Zero SM/wrap invocations afterwards** while
  the run continued to frame ~172k.
- Meanwhile `read_step_cb` 0x801985C4 IRQ callbacks continued to
  frame ~6101, and new read sessions kept starting (`cd_read_setup`
  0x80198C90 + `cd_read_open` 0x80198908 + `init_done_cb` 0x80198590 at
  frames 5726/5762/5778/5926/5940/5990/6090) — the CD file-read
  machinery stayed alive; only the channel-entry SM lost every driver.
- End state identical on both instances: fc688=1, fc69c=0, fc6b0=0,
  `*0x1F801098`=0x200 (bit24 clear → even a hypothetical SM run would
  take the non-rearm wait path), fc68c=0, bc848=bc84c=0,
  SM status `0x801BCC88`=0xA, producer idx 2 / finalize idx 0.

## 5. Mechanism of the starvation

Not a missed single set — a **driver-set collapse**:

1. During the stream/0x0E window the SM is kicked per CD IRQ
   (bc84c=sm_wrap) and per DMA-channel completion (0x8019AF7C table).
   Each run that sees DMA-ch1 busy writes fc68c=1 → next frame's
   tile_stream_pump/hp_bar_tick runs it once more → repeat.
2. Around frame ~5704 the last SM run(s) exited without re-arming
   (channel busy bit already clear → `cd_result_wait_b` path → status,
   no fc68c write). After that: bc84c had been re-parked to
   `read_step_cb` for ordinary file reads (fntrace shows read sessions
   continuing), the dispatcher bit for the SM's table slot was no
   longer pending, and fc68c was 0 → **no path can invoke the SM
   again** → producer idx frozen at 2, finalize idx 0 → no ch4
   doorbell → no `DeliverEvent(0xF0000009)` → BIOS `B0:11` wait wedge.

The flag logic itself is faithful: fc68c is a consumed-once re-arm
armed only while the DMA channel reports busy. There is deliberately
no external setter — the SM is its own setter.

## 6. Faithful fix location

**Not** `menu.c`'s fc68c gate/clear sites and **not** the SM delegation
— all faithful. The starvation is upstream: whatever should keep the
channel-entry session (and thus an SM driver) alive after frame ~5704
stopped. Concretely, the last SM runs left `finalize idx = 0` with
`producer idx = 2` — pending entries were never finalized, matching
`proof/cd_finalize_gate.md`'s arena-overwrite / seq-tag-gate theory
(`u16@entry+6 == u16@entry+4+1` gate in `cd_mc_state_machine`'s entry
walk never passing because the `0x80010000` scratch arena was
overwritten by file-load DMA payloads). The fix belongs in the
produce/finalize ordering or the entry-tag validation — i.e. the SM's
entry-walk gate inputs — not in the fc68c pump flag.

Open follow-up: determine why the SM's last invocation(s) at frames
5703-5704 did not finalize the pending entries (entry-walk early-out:
helper `cd_result_wait_b` ret==5, resp byte bit2, or entry tag/seq
mismatch vs the overwritten arena), and why the 0x8019AF7C dispatcher
bit / bc84c park chain did not re-kick it. `wtrace`/`fntrace` harness
on :8985 (still wedged) can arm 0x801FC6A4/0x801FC6A8/0x801BCC88 and
the 0x801BDDF4 table to catch the exact last SM exit path on a rerun.
