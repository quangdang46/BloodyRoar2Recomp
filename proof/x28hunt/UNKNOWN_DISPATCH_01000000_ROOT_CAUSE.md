# Unknown-dispatch 0x01000000 — root cause (post-x28 fix)

**Status: FIXED in `game/menu.c` (uncommitted working-tree change). Runtime-verified.**

## Symptom

After the `fighter_pair_update`/`camera_aim_offset` host-stack fix
(commit `095785e`), a VERSUS run reached `gs=8 ph=8` (fight phase) and then
fail-fasted on an unknown dispatch to guest `0x01000000`:

```
psx_unknown_dispatch → psx_dispatch_impl → call_guest4
  → prim_list_gte_invoke (game/battle.c:12040)
  → battle_flow_state_machine (game/menu.c, case 8)
  → clean_try_dispatch (0x8016FCC4)
```

`dispatch_tail` showed the last valid clean render addrs (`0x801864DC` …)
then `0x01000000` with `$ra=0x80126664`, `$a0=0`, `$a1=0x801E62A0`.

## Root cause — decompiler-port bugs in `battle_flow_state_machine`

The crash was **not** the x28 stack overwrite and **not** a bad record
callback. It was wrong arguments handed to `prim_list_gte_invoke`, which
then iterated a garbage "record list" until a `fn` field of `0x01000000`
was dispatched. Three distinct mistranslations in `game/menu.c`:

### 1. Word cells defined as `uint8_t` (flag truncation + pointer truncation)

`_DAT_1f800100/148/154/250/25c` were `#define`d as `*(uint8_t *)…`.
The guest accesses all of them with `lw` (word). Consequences:

- `_DAT_1f800154 & 0x200 / 0x400 / 0x8000000` and `_DAT_1f80025c …` were
  **always 0** (a `uint8_t` can never hold those bits) → the `else`
  branches (secondary render-record path) were dead and branch selection
  was wrong every frame.
- `_DAT_1f800100` holds a **guest pointer** (`0x801beff0`, the P1 prim
  header). As `uint8_t` it read only the low byte `0xF0`, so
  `uVar3 = _DAT_1f800100` produced `param_1 = guest_at(0xF0)` — a wild
  pointer into low guest RAM. `param_1[7]` then read an arbitrary word as
  the "record list" address and `prim_list_gte_invoke` walked garbage
  until `call_guest4(0x01000000,…)` fail-fasted.

Guest proof: `generated/SLES_017.22_full_31.c` loads the flag words with
`psx_cyc_load_word` at `$s3+0x6C` (`0x801703D8`) and `$s3+0x60`
(`0x801703D8`-region), and the pointer cell with `lw 0x18($s3)`
(`0x80170498`) = `*(0x1F800100)`.

Fix: all five defines widened to `*(uint32_t *)…` (matches the
already-correct `battle.c` defines, `game/battle.c:61-69`).

### 2. Byte offsets emitted as `uint32_t*` element indices

`_DAT_1f800110`/`_DAT_1f800218` point at a table of **0x24-byte (9-word)
render records** (verified live: rec0 @`0x801F5258`, rec1 @`+0x24`,
stride `0x24`). The guest indexes it with **byte** offsets; the clean
port used the same numbers as `uint32_t` indices → 4× too far:

| guest insn | byte offset | clean (buggy) | clean (fixed) |
|---|---|---|---|
| `lw 0x10($v0)`  | +0x10 | `+ 0x10` u32 = +0x40 | `guest_at(_DAT_1f800110 + 0x10)` |
| `lb 0x24($v0)`  | +0x24 | `[0x24]` u32 = +0x90 | `*(u8*)guest_at(_DAT_1f800110 + 0x24)` |
| `lw 0x34($v0)`  | +0x34 | `+ 0x34` u32 = +0xD0 | `guest_at(_DAT_1f800110 + 0x34)` |
| `lb 0($v0)`     | +0x00 | `*(u32*)guest_at(...)` | `*(u8*)guest_at(_DAT_1f800110)` |

Guest proof (`generated/SLES_017.22_full_31.c`):
- `0x80170414: lw $a0,0x10($v0)` / `0x80170404: lb $a3,0($v0)` — call-A args.
- `0x80170430: lb $a3,0x24($v0)` / `0x8017043C: lw $a0,0x34($v0)` — branch-2.
- `0x80170660/0x80170670` — identical P2 offsets on `_DAT_1f800218`.

### 3. `param_4` (record count) width

Guest passes `param_4` in `$a3` via `lb`/`lbu`+`sll24/sra24` = a **signed
byte**. The clean port passed the whole `uint32_t` word (e.g.
`0x09140F13` → `param_4 = 152M` instead of `0x13 = 19`), so even a correct
`param_1` would iterate ~152M headers, walking `param_1` off the table.
Fix: `(int)(int8_t)` on the byte loads (`DAT_1f8000ee`, `DAT_1f8001f6`,
and the struct `+0x24`/`+0x00` bytes).

## Live data that confirmed the layout

```
_DAT_1f800110 cell (0x1F800110) = 0x801F5258   (P1 record table)
rec0 @0x801F5258: 09140F13 0D 80077110 80077158 801BEFF0 801BF860 …
rec1 @+0x24      : 09190F18 0D 80077134 8007718C 801BF428 801BFC98 …
+0x10 -> 0x801BEFF0 (prim hdr, hdr[7]=0x8004E940 record list)
+0x24 byte -> 0x18 (count), +0x34 -> 0x801BF428 (record[1] hdr)
_DAT_1f800100 cell = 0x801BEFF0  (same hdr cached in scratchpad)
```

## Runtime verification (port 5704, post-fix binary)

- `tools/versus_nav_final.py 5704` → `gs=8 ph=8` at frame 20195,
  `VERSUS FIGHT LIVE` (frames advancing).
- Sustained observation: frames 20195 → 51000+ (~30 k guest frames),
  `gs` stayed `8`, mode bytes `0x9b=2 / 0x9c=1` (2P versus).
- Button-mash on both pads drove **multiple completed rounds**: `ph`
  cycled `8 (fight) → 0/1 (round end) → 3/4 (next-round intro) → 8`
  repeatedly — a live versus match, not a stall and not an idle phase.
- No `psx_last_run_report.json` unknown-dispatch entry for this run
  (the only report present is a `signal_sigterm`/`atexit` artifact of a
  killed launcher attempt at frame 0).
- Process healthy (~86 % CPU, >3 min CPU time) throughout; no SIGSEGV,
  no `0x01000000` dispatch.

For contrast, the pre-fix run fail-fasted at frame 10268 — within the
first seconds of `ph=8` — on the `0x01000000` dispatch.

## Files changed

- `game/menu.c` — five `uint8_t→uint32_t` define widenings
  (`_DAT_1f800100/148/154/250/25c`), four `prim_list_gte_invoke` call
  sites corrected (P1 @~line 820/826, P2 @~line 861/867), signed-byte
  `param_4` casts. `generated/` and `src/clean_table.c` untouched.

## Notes / follow-ups

- The same "byte offset used as `uint32_t*` index" and "word cell defined
  as `uint8_t`" mistranslation classes may exist elsewhere in `game/*.c`.
  `battle.c` already uses the correct `uint32_t` defines and
  `guest_at(addr + byte_off)` idiom — it is the reference convention.
  A systematic audit of `*(uint8_t *)g__DAT_*` defines against guest `lw`
  sites is worthwhile but was out of scope here.
- `param_5`/`param_6` of `prim_list_gte_invoke` are host-side args
  (GTE matrix guest addresses `0x1f800028`/`0x1f80033c`) consumed inside
  the clean body — they do not traverse `call_guest4`, so no ABI issue.
