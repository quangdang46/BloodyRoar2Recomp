# CD wedge — clean-C mistranslation hunt (static diff vs generated)

Read-only investigation, 2026-11-21. Diffed every remaining suspect in
`game/cd.c` / `game/save.c` instruction-class-by-instruction-class against
`generated/SLES_017.22_full_*.c`. **Four definite mistranslations found**,
one of which (`op_frame_advance` unconditional clear-tail) is a strong
wedge candidate. No source edited.

Live evidence pulled from still-wedged instance **6020** (frame 455935,
cdrom seq frozen at 68207 — same wedge as `cd_seek_blocker.md`):

- BE7xx fn-ptr table (`mem_words 0x801BE780`):
  `BE780=0x801A38B8` **cd_cmd_queue_drain**, `BE784=0x801A3850`
  cd_cmd_rec_clear, `BE788=0x801A39BC` cd_cmd_data_byte,
  `BE78C=0x801A3A78` **cd_cmd_rec_scan**, `BE790=0x801A3CF4`,
  `BE794=0x801A3D2C`, `BE798=0x801A3D84` cd_cmd_state_commit,
  `BE79C=0x801A41D8`, `BE7A0=0x801A3E94` cd_cmd_state_rx,
  `BE7A4=0x801A39AC` qbyte_take, `BE7B4=0x801FF080` (rec table base),
  `BE7B8=1`.
- Wedge rec0 `0x801FF080`: rec[52..55]=`00 00 01 00` (rec54=1,rec55=0),
  rec[68..71]=`00 02 01 00` (**rec70=1**), rec[72..75]=`00 01 05 00`
  (**rec73=1, rec74=5**), rec[76]=0x000667FD (frame counter advancing),
  datptr(rec+48)=0x801BEFA8, cmdptr(rec+60)=0x801FEFF0.

---

## MISTRANSLATION 1 (prime wedge suspect) — `op_frame_advance` clear-tail is unconditional

**Clean:** `game/save.c:9170-9218` (`op_frame_advance`, guest 0x801A40F8,
wrapper `src/clean_table.c:4680` `wrap_801a40f8` → `guest_at(gpr[4])`).
**Generated:** `br2eu_func_801A40F8`, `generated/SLES_017.22_full_50.c:9196`.

Generated CFG (verified block-by-block):

```
entry: rec[76]++ (unconditional, delay-slot store @0x801A4118)
  rec70==0            -> block_801A4190  (TAIL)
  rec70==1            -> block_801A4128: rec74<11 -> 4164 rec74++ -> j 41C8 END
                                              rec74>=11 -> 413C {rec73=2,rec70=255} -> j 41C8 END
  rec70>=2 (other)    -> block_801A4150: rec74<11 -> 4164 rec74++ -> j 41C8 END
                                              rec74>=11 -> 416C: rec73==0 -> 4190 (TAIL)
                                                           rec73!=0 -> jalr *0x801BE784(rec,live a1), ra=0x801A4190 -> TAIL
TAIL block_801A4190/41A8: if **(rec+60)==0xF3 return;
  else datptr=*(rec+48): datptr[0]=0xFF, datptr[1]=0, rec[232]=0, rec[53]=0
```

The TAIL (cmd==0xF3 check + datptr-head/rec[232]/rec[53] clear) is reached
**only** from: rec70==0 entry, the rec70>=2 && rec74>=11 paths
(directly when rec73==0, or as the jalr-BE784 return address when
rec73!=0). It is **never** reached for rec70==1 (any rec74) or for
rec70>=2 with rec74<11 — those exit at block_801A41C8.

**Clean C runs the TAIL unconditionally** after the state dispatch —
including after every `rec74++` early path and after the rec70==1
exhaust path (`rec73=2, rec70=0xff`). For the wedge record (rec70==1,
cmdptr→`00 5a ff ff`, *cmdptr=0x00 ≠ 0xF3) every call writes
`datptr[0]=0xFF, datptr[1]=0, rec[232]=0, rec[53]=0` — stomping exactly
the fields `cd_cmd_state_rx` consumes (nibble latch rec[232], payload
length rec[53], response head datptr[0..1]).

**Why it's on the wedge path:** `op_frame_advance` is invoked by
`cd_cmd_queue_drain` (`game/cd.c:6064`) as the selector!=0 pre-op, paired
with `qbyte_take`. `queue_drain` itself is `*0x801BE780`, invoked by
`cd_cmd_queue_service` (`game/cd.c:6031`) whenever `dev_cmd_seq` returns
0 ("still busy") with a0=0xffff — i.e. potentially every service pass
during CD load — and by `dev_cmd_round_dispatch`'s negative-result error
path. Live rec[74]=5 (nonzero, <11) on the wedge record is consistent
with `op_frame_advance` having run on it (state_rx's nibble-0 early path
returns before `s0[74]=0` at `game/cd.c:5876`, so rec74 is not re-zeroed
on this record). On the 5800 specimen rec74 was 0x0A — one increment
from the rec70==1 exhaust transition (rec73=2, rec70=255).

**Fix semantics:** restructure so the tail runs only on the generated
paths — early `return` after each `rec74++` and after the rec70==1
exhaust store; `goto tail` only for rec70==0 and the rec70>=2 &&
rec74>=11 cases (rec73==0 direct; rec73!=0 after the BE784 jalr, which
returns into the tail). Do NOT reorder the rec[76]++ (unconditional).

## MISTRANSLATION 2 — `cd_cmd_rec_scan` t1 bound inverted

**Clean:** `game/cd.c:5931` — `t1 = (s0[52] < 7) ? 6 : s0[52];`
**Generated:** `br2eu_func_801A3A78`, `generated/SLES_017.22_full_50.c:3621`.

Generated @0x801A3AB4-0x801A3ACC: `lb v0,52(s0)`; `sltiu v0,v0,7`;
`li t1,6` (delay slot); `beq v0,zero -> 0x801A3ACC` (skips reload).
Fallthrough 0x801A3AC8: `lb t1,52(s0)`. So:
**t1 = (rec[52] < 7) ? rec[52] : 6** — i.e. capped at 6, equals rec[52]
when rec[52] ≤ 6.

Clean has the ternary **backwards**: gives t1=6 when rec[52]<7 and
t1=rec[52] (up to 255) when rec[52]≥7. t1 bounds BOTH inner match loops
(`for i < t1` over `aux93+i` = rec+93+i and `stream+i`). With the bug,
rec[52]≥7 makes the loop read rec[93..93+254] — past the 240-byte record
into the next record — and stream bytes far out of bounds, computing
wrong BE7C8 index accumulation (`*0x801BE78C` path via `cd_cmd_issue_poll`
`game/cd.c:5707`) and wrong aux87[] match marks. rec_scan is `*0x801BE78C`
(live table dump above) — invoked through `cd_cmd_issue_poll`'s
flag==0 path every pump.

**Fix semantics:** `t1 = (s0[52] < 7) ? s0[52] : 6;`

## MISTRANSLATION 3 — `cd_cmd_data_byte` two divergences

**Clean:** `game/cd.c:5750-5769`. **Generated:** `br2eu_func_801A39BC`,
`generated/SLES_017.22_full_50.c:2990`.

(a) **Missing mode==0x4d OOB case.** Generated: mode!=0 dispatches at
0x801A39D0 — `mode==0x4d` → block_801A3A2C: `idx<rec[54]` (signed) else
return **0xFF** (delay-slot `li v0,0xFF` @0x801A3A3C); other nonzero
modes → block_801A3A4C: same bounds check else return **0**. Clean
collapses all nonzero modes to `return 0` — the 0x4d→0xFF distinction is
dropped. Callers may treat 0xFF as a distinct sentinel.

(b) **mode==0 aux guard signedness.** Generated @0x801A39E4: `slti
(idx,6)` **signed** — `(int32_t)idx < 6`. Clean @`game/cd.c:5757`:
`idx < 6u` **unsigned**. idx = rec[69]-3 wraps to 0xFFFFFFFD.. when
rec[69]<3: generated treats it as negative (<6 → reads aux
rec[87+idx]=rec[84..86] and may early-return 0); clean skips the guard
and proceeds to the rec[52] compare returning `ptr40+idx` (ptr40-3!).
Diverges whenever rec[69] ∈ {0,1,2}.

The mode-0 rec[52] compare and the nonzero-mode rec[54] compares are
correctly signed in clean already; ptr40/ptr44 indexing verified.

**Fix semantics:** (a) add `if (mode == 0x4d) { if ((int32_t)idx >=
(int32_t)r[54]) return 0xff; }`-equivalent before the shared nonzero
tail (or split the tail); (b) change `idx < 6u` to `(int32_t)idx < 6`.

## Verified faithful this pass

- `cd_cmd_queue_service` (0x801a189c) vs `br2eu_func_801A189C`
  (full_49.c:16503): bit1-of-(BE7E4)+10 early clear, BE7E8=1 delay-slot,
  BE7D0/BE7D4-gated counter bumps @0x801FEFD0/D4 (<150 signed),
  BE7B8 gate, signed BE7D4<BE7D0 exits, dev_cmd_seq call
  `BE7B4+BE7D0*240`, BE780(0xffff) on 0, BE7C8=0, drain loop
  `while BE7D4>=BE7C0: dev_cmd_round_dispatch(BE7B4+BE7C0*240)` with
  per-iteration reload, `sh 0x88 -> (BE7E4)+14`, v0=0 all paths. ✓
- `cd_cmd_queue_drain` (0x801a38b8) vs `br2eu_func_801A38B8`
  (full_50.c:2186): sel==-9 skip / sel!=0 → 0x801A40F8+0x801A39AC /
  sel==0 → BE7D8[head]=0; BE7C4=0, sh 0→(BE828)+10, head=BE7C0+1 store,
  signed BE7D4<newhead → ret 1 (a1=0xffff), else dev_cmd_seq
  `0x801FF080+newhead*240` loop while v0==0 (a1=0xffff after iter 1). ✓
- `op_tag_dispatch` (0x801a24dc) vs `br2eu_func_801A24DC`
  (full_49.c:26407): rec70==3→jal 0x801A3048(a1=rec[228]); <4(signed
  slti, u8-safe)→==2→jal 0x801A3034(a1 live); ==4→jal 0x801A3088
  (a1=rec[71]); else return. Matches `game/save.c:8977`. ✓
- `qbyte_take` (0x801a39ac): rec[56]=rec[55]; rec[55]=0. Matches
  `game/save.c:9872`. ✓
- `op_frame_advance` dispatch skeleton (rec76++, rec70/rec74/rec73
  lattice, BE784 jalr) — matches except the tail unconditionality
  (Mistranslation 1).
- `dev_cmd_seq` (0x801a1b50) and `dev_cmd_round_dispatch` (0x801a1e84):
  CPS-split across many generated pieces; clean versions carry
  insn-level verification comments (`game/save.c:9959-10130`,
  `:10332-10419`). Entry piece spot-checked (BE7E4 dev setup stores,
  rec[0xE8]==8→0x50/0x91 select, BE7C0 slot, BE7D8+slot*4 signed count,
  BE7E4 reload + slot==0?0x1003:0x3003) — consistent; not exhaustively
  re-diffed (SIO/memcard path, lower wedge probability).

## Wedge-mechanics assessment

The armed record needs rec[55]≠0 (class tag) and rec[70] to advance past
1. rec[55] is stamped only by `cd_cmd_rec_init` (0x801a3014), reached
via `cd_cmd_state_commit` b70==1/254 — and state_commit fires only via
`*0x801BE798` calls (cd_cmd_flush_a/b sub-record walk, op paths). The
per-frame pump is: queue_service → dev_cmd_seq → BE7A0(state_rx)+
BE7A4(qbyte_take) per drained record; queue_drain (BE780) adds
op_frame_advance+qbyte_take pre-ops when dev_cmd_seq reports busy.

Mistranslation 1 corrupts rec[232]/rec[53]/datptr on exactly the
rec70==1 armed record whenever the drain pre-op runs — a timing-dependent
per-frame race against state_rx's consumption of those fields. That is
the best static explanation for the ~2/3 intermittent wedge: whether the
clear lands between the response deposit and the state_rx commit
decides whether the record ever reaches state_commit/rec_init.

Mistranslations 2-3 are real but secondary: rec_scan feeds BE7C8 match
accounting (wrong bounds → wrong matches), data_byte's 0x4d/idx<3 edges
return wrong bytes to response consumers.

## Recommended next steps (parent decides — no edits made)

1. Apply Mistranslation 1 fix in `game/save.c` `op_frame_advance`
   (highest value; on the exact wedge record state).
2. Apply Mistranslations 2-3 in `game/cd.c` (`cd_cmd_rec_scan` t1,
   `cd_cmd_data_byte` mode-0 signed guard + 0x4d case).
3. Rebuild + runtime-verify: wedge rate should drop; watch rec[70]/
   rec[55] on 0x801FF080/0x801FF170 and DeliverEvent(0xF0000009,0x20)
   in bioscall_dump.
4. If wedge persists after fixes: remaining unverified surface is the
   dev_cmd_seq CPS-piece chain (save.c:10004+) and the delegated
   generated response path (`cd_response_process` 0x80196370,
   `cd_cb8_dispatch` 0x8019AE68) — runtime working-vs-wedged record-byte
   diff still applies (instances 6020 wedged / others may succeed).

## Evidence index

- Live dumps (6020): `mem_words 0x801BE780 count 16` (fn-ptr table),
  `mem_words 0x801FF080 count 60` (rec0), `cdrom_state` (seq 68207
  frozen), `frame` 455935.
- Generated (read-only): full_49.c:16503 (queue_service), :18566+
  CPS pieces (dev_cmd_seq), :21495 (dev_cmd_round_dispatch first piece),
  :26407 (op_tag_dispatch), :26956 (0x801A2560); full_50.c:1876
  (rec_clear), :2186 (queue_drain), :2931 (qbyte_take), :2990
  (data_byte), :3621 (rec_scan), :6245 (state_commit), :7256 (state_rx),
  :9196 (op_frame_advance).
- Clean (read-only): game/cd.c:5595-6083 (chan_job_pump..queue_drain),
  :5626-5633 (rec_init), :5719-5769 (rec_clear/data_byte),
  :5779-5808 (state_commit), :5825-5907 (state_rx), :5922-5991
  (rec_scan), :6003-6043 (queue_service), :6054-6083 (queue_drain);
  game/save.c:8977-8990 (op_tag_dispatch), :9170-9218
  (op_frame_advance), :9872-9878 (qbyte_take), :9959-10130 (dev_cmd_seq),
  :10332-10419 (dev_cmd_round_dispatch); src/clean_table.c:4664-4683
  (wrap_801a3a78/801a3d84/801a3e94/801a40f8).
- Helper: /tmp/dbg.py JSON-lines client; condensed generated extracts
  /tmp/gen_*.txt.

No source files were modified.
