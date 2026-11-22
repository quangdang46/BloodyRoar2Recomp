# cd_mc_state_machine — full disassembly + last-activity capture

Date: static disasm of `SLES_017.22` + live wtrace/fntrace on fresh
instance **debug port 8993** (same wedge reproduced: `gs=8 ph=03`,
producer `0x801FC6A4`=2, finalize `0x801FC6A8`=0, `0x801FC68C`=0,
`0x801BC848`=`0x801BC84C`=0, CD idle-paused `stat=0x02`).

## 1. SM layout (guest 0x80199440, 1640 bytes + CPS continuations)

The generated unit is split into CPS fragments; the logical body spans
0x80199440–0x80199D58 plus helpers 0x80199D5C (memcpy) / 0x80199D88
(async channel copy) / 0x80199320 (table adjust) / 0x801990F0
(`cd_dir_entry_finalize`).

Globals (all `lui 0x8020` + negative offset):

| Addr | Role |
|---|---|
| 0x801FC69C | reentrancy guard — `beq ==1` exits at entry |
| 0x801FC688 | session-active flag |
| 0x801FC6B0 | retry/error mode flag |
| 0x801FC6A0 | retry/sequence counter (bumped on error paths) |
| 0x801FC6A4 | **scan index** — `table + idx*0x20` → `0x801FEB18` |
| 0x801FC6A8 | **finalize index** — set = FC6A4 on drain path |
| 0x801FC6C0 | entry-table base pointer (=0x80010000) |
| 0x801FC6C4 | table stride/count (=0x20) |
| 0x801FC6B4 | pending-count watermark |
| 0x801FC6B8 | callback-pending flag (set 1 before `jalr *0x801FEB14`) |
| 0x801FC680/684 | expected sequence pair (u32/u16) |
| 0x801FC694/698 | channel-match fields |
| 0x801FC6BC | per-index 0x7E0-byte payload pointer |
| 0x801FC6F8 | mode flag (set by `cd_mode_set_0e` per arg&0x20) |
| 0x801FEB14 | entry-completion callback (`jalr`) |
| 0x801FEB18 | current entry pointer |
| 0x801BCC40..74 | register-pointer table: BCC50=0x1F8010F0 (**DPCR**),
  BCC54=0x1F8010F4 (**DICR**), BCC58=0x1F801098 (MDEC-out CHCR),
  BCC40=0x1F801800 (CD regs), BCC70=0x1F8010D8 (DMA ch6+8) |
| 0x801BCC88 | status-out word (codes 3,4,5,6,7,8,9,0xA) |

Entry gates decoded from disasm:

- `entry+0 u16 == 0` → free: issue mailbox command, consume response.
- `entry+0 u16 != 0` → status 4 exit (+FC6A0 bump if FC6B0).
- `entry+0 == 0x160` → special path (0x801997A4).
- Sequence gate at 0x80199BC4: `lhu(entry+6)-1 == lhu(entry+4)` →
  **fresh entry**: sets `FC69C=1`, does the 0x1F8 payload copy
  (0x80199D5C sync or 0x80199D88 async), bumps FC6A0, then at
  0x80199CF0 writes `entry+0=3`, advances FC6A4, and — because
  FC6B0!=0 && FC69C!=0 — calls **`cd_dir_entry_finalize` (0x801990F0)**.
- Mismatch → alternate copy path (0x80199C78), FC6A4 still advances,
  **finalize NOT called**.
- Drain path 0x80199AF0: `FC6A8 = FC6A4` (finalize=scan), status 0xA.
- Reset path 0x80199844 (seq-pair mismatch vs FC680/684): zeroes
  FC680/FC684, `jal 0x80199320(finalize, scan-finalize)`, then
  `FC6A4 = FC6A8`, `entry+0=0`, status 6.

Defer path (the ONLY fc68c=1 writer, 0x8019949C delay slot): reached
when `FC688!=0` AND `*(*(0x801BCC60)) & 0x01000000` (MDEC-out DMA busy).
Writes FC68C=1, bumps FC6A0 if FC6B0, status=1, exits — re-polls next
frame via the tile_stream_pump gate.

## 2. Drivers — confirmed three, all verified silent at wedge

1. `tile_stream_pump` (menu.c:2277): `if (f4738 && fc68c) { SM(); fc68c=0; }`
2. IRQ session callback `bc84c = cd_mc_sm_wrap` (0x80198FE4 → `jal SM`),
   installed by `cd_mode_set_0e` (0x80198F60) when arg has bit 0x100 —
   registers 0x801990F0 + 0x80198FE4 via 0x801961A8/0x80195D78.
3. DMA dispatcher 0x8019AF7C (SM ra=0x8019B024).

## 3. Runtime capture (port 8993, armed before nav)

wtrace_range on `0x801FEB10-0x801FEB20` recorded every SM entry-ptr
store (pc=0x80199524). 118 writes, f=3350→3441:

- SM scanned entries in repeating groups of ~3 indices per 2–3 frames:
  `{2,3,4} / {11,12,13} / {17,18,19} / {26,27,28}` — the active
  streaming channels — for ~90 frames.
- **Last write seq=2000, f=3441, entry idx=28 (0x80010380).**
  After that: zero FEB18 writes → SM never reached the entry walk
  again for the remaining ~13k+ frames.

fntrace (SM cluster + resp_pump/read_step/dma_disp armed): after
~f=3900 the only activity is `resp_pump`/`read_step`/`getresp`/
`dma_disp` — the transient file-read path — with **zero** SM-family
hits. CD command history shows normal SEEK/READ/PAUSE/SetMode cycling
continuing to f=3905, then idle (`stat=0x02`, Pause consumed).

## 4. Wedge state (identical on 8992/8993)

```
FC688=1  FC68C=0  FC69C=0  FC6A0=0  FC6A4=2  FC6A8=0  FC6B0=0
FC6B4=0xFFFFFFFF FC6B8=0 FC6BC=0x80010BE0 FC6C0=0x80010000 FC6C4=0x20
FEB14=0 (callback)  FEB18=0x80010020 (stale entry[1])
bc848=0 bc84c=0 bcba8=0 bcbac=0
```

Entry table at 0x80010000 holds the tile-stream directory content
(offset table + packed records) — consistent with `tile_stream_open`
→ `cd_dir_table_set(_DAT_801f4754=0x80010000, 0x20)` being the
legitimate owner, not corruption.

## 5. Refined wedge model

The SM was healthy until ~f=3441, then **stopped being invoked** —
not "ran and failed". For an invocation all three drivers must be
quiet simultaneously:

- per-frame gate: dead because the last SM run exited via the
  work path (fc68c only re-arms on the MDEC-out-busy defer path);
- IRQ `bc84c=sm_wrap`: dead because the session callback cell is 0 —
  the mode-0x0E window that installed it ended (per-op read
  callbacks own bc84c during file reads);
- DMA dispatcher: channel idle after f≈3443.

With `scan(FC6A4) != finalize(FC6A8)` entries remain unfinalized →
`cd_dir_entry_finalize` never runs for them → the BIOS event the
game `WaitEvent`s on is never delivered → deterministic wedge at
`gs=8/ph=03`.

## 6. Open question (next step)

Was the last SM invocation supposed to leave re-arm state (a dropped
CD IRQ / response around f=3441–3443 — command history shows a
Pause+SetMode pair at f=3443 right at SM death), or did the game
legitimately leave mode-0x0E and the pending entries are abandoned
by design — meaning the wedge is a *different* missing event?

Discriminator: compare the same point on the Beetle/DuckStation
oracle (`tools/nav_to_fight_oracle.py`, port 4371) — does the SM
keep running past the equivalent frame, and what does bc84c/fc68c
look like there?
