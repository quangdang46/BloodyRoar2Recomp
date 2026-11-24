# cd_read_poll (0x80198E2C) — third port attempt: MUST-DELEGATE verdict

## Verdict

`cd_read_poll` **cannot be faithfully clean-ported** in its current form.
A third faithful CFG reproduction (18 blocks verified insn-by-insn,
`br2_irq_poll()` + `br2_bail_pending()` once per loop iteration, both
nested `jal` sites — `cd_read_open` 0x80198908 and
`cd_result_wait_b_fwd`→`cd_result_wait_b` 0x80195D44 — left delegated)
compiled cleanly, ran **~1519 frames**, then died deterministically:

```
FATAL: FAIL-FAST unknown dispatch: addr=0x8019A5D8 phys=0x0019A5D8
       ra=0x801265F4 a0=0x2880064B a1=0x00000000
```

`0x8019A5D8` = `vsync_wait_n` (its code bytes at that address had been
overwritten with data); `ra=0x801265F4` is in battle/init code.

A same-binary control with `cd_read_poll` delegated to generated
(`br2_call_generated(0x80198e2c,…)`) ran clean past **frame 24000+**
through the same content — the port is the trigger.

## Root-cause chain (all verified at runtime)

1. **Deterministic crash at frame 1519**, during the battle-intro
   resource load: `battle_intro_flow` (menu.c:2135) →
   `menu_res_load_sm` (save.c:4327) → `save_channel_init_commit`
   (save.c:6432) → `cd_file_load_sm(0x13, guest_of(tag_table_get()))`
   loading the tag-table file to guest `0x801C47C0`.

2. **The CD read completion is wrong under the clean poll loop.**
   `gwatch` (mprotect write-watch — the only surface that sees clean-C
   `guest_at()` stores) + `decomp_trace` ring show:
   - heap cursor `*(0x801C47B8)` became `0xA8810B4C` (truncated host
     pointer) via `save_channel_init_commit` save.c:6432 cursor-align
     write — i.e. it was already wild coming out of the decode loop;
   - `stream_block_decode` → `mode3_dispatch` → `block_copy_ret_len`
     ran its 0x64-iteration slot loop with `dst=0xA8810B4C`,
     `src=0x499C50F7`, `len=0` for every tag (decomp_trace seqs
     ~264-520, all at the same guest cycle = one clean-C burst);
   - decoded heap blocks around `0x801B563C` contain
     `0x28xxxxxx`/`0xa8xxxxxx` values = **truncated host pointers**,
     where master holds self-relative offsets (`0x00255601`-style).

3. **`data_relocate` (save.c:9623, guest 0x801AA96C) is the spray
   amplifier, not the origin.** Its fixup loops read wild count/base
   fields from the corrupt block header and emit `*(u32*)guest_at(pa)
   += a0` writes — gwatch rip `0x1055055AC`→`data_relocate` logged
   464 in-range hits in just 8 sample windows, `oor=675382` page
   faults, i.e. **hundreds of thousands** of `+=` writes across
   ~0x80198000-0x801C0000+. Verified overwriting:
   - code at `0x8019A5D8` (`vsync_wait_n`) → wild dispatch crash;
   - frame counter `*(0x801BDDE0)` → `0x2880064B` (was `0x5E87`-style
     sane value on master).

4. **Why the port can't express this:** per AGENTS.md §4, the CD
   deferred-interrupt coroutine (`cd_ctx_restore → cd_chan_full_init →
   cd_intc_dispatch`) needs a **live guest resume PC / $ra caller
   token** that only the generated CPS frame supplies. `br2_irq_poll()`
   delivers the IRQ but the coroutine resumes against stale
   `cpu->gpr` → INT2 completion for the file load lands wrong/late →
   the tag-table file region keeps stale host-pointer garbage →
   decode → relocate spray. This is the same starvation class that
   wedged the first two attempts (vsync_wait_n spin / frozen
   `DAT_801bdde0`), just manifesting one stage later as memory
   corruption instead of a livelock.

## State

- `game/cd.c`: `cd_read_poll` is a `br2_call_generated(0x80198e2c)`
  delegation with the full post-mortem comment; faithful CFG port
  preserved in git history/stash.
- `src/clean_table.c`: regenerated — 1356 wrappers, `0x80198E2C`
  skipped (self-delegate) so generated callers dispatch straight to
  `br2eu_func_80198E2C`.
- Build clean; runtime verified past the crash frame (frame 3564+ and
  climbing at check time).

## Reproducing the diagnosis

- Launch headless `--debug-port N`; the whole boot→crash takes ~4 s
  (frame 1519), so arm probes **within ~1.5 s of launch**.
- `gwatch {"addr":"0x801C47B8","len":16}` etc. catches clean-C writes
  (wtrace does NOT — clean stores bypass `psx_write_*`).
- `decomp_trace` shows the decode ring; `read_ram` diff vs a master
  instance isolates corrupted cells; host RIPs symbolize via
  `atos -o build/BloodyRoar2_Recompiled -l <__TEXT base>` (slide =
  runtime `__TEXT` − `0x100000000`).
