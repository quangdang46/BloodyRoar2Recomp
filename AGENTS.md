# AGENTS.md — Bloody Roar 2 clean-room RE mission

> Read `psxrecomp/CLAUDE.md` first — it is the faithful-core constitution
> (Architecture A locked, no interpreter, no stubs, LLE baseline). This file
> is the **project mission + workflow** that sits above it.

---

## 0. THE MISSION (non-negotiable)

**Reverse-engineer Bloody Roar 2 (PSX / SLES-01722) into ONE clean,
readable, fully-working C source tree.**

The deliverable is **clean source code** — real filenames, real function
names, real types, readable control flow — that reproduces the game exactly
and **runs every feature**. It is NOT:

- ❌ the raw recompiler output in `generated/` (that is "encode" — machine
  transliteration, not a source you can read or maintain),
- ❌ documentation/markdown instead of code,
- ❌ stubs, `return 0` guesses, or half-ported functions,
- ❌ a second parallel source tree — there is **ONE** source of truth.

`generated/` is a **temporary authoritative fallback** only for functions
not yet faithfully ported to clean C. The long-term goal is to drive
`generated/` to zero load-bearing coverage by porting every subsystem into
`game/*.c`.

---

## 1. SOURCE LAYOUT — know which tree is which

| Path | Role | Editable? |
|------|------|-----------|
| `game/*.c`, `game/*.h` | **THE clean source.** The deliverable. Real names (`cd_file_pump`, `action_commit`, `anim_lerp_vec3`…). | ✅ yes — this is the work |
| `generated/SLES_017.22_full_*.c` | Recompiler raw output (MIPS→C). NOT the deliverable. | ❌ never hand-edit (fix the recompiler, not the output) |
| `src/clean_table.c` | Auto-generated dispatch: guest addr → clean fn. | ⚠️ regenerate only, never hand-edit |
| `game/fnmap.c` | Auto-generated guest-addr → host-fn map. | ⚠️ regenerate only |
| `game/renames.map` | guest addr → clean name (`80126870 cd_file_pump`). | ✅ the naming authority |
| `psxrecomp/` | The recomp runtime + recompiled BIOS + hardware sim. | per CLAUDE.md |

`game/` currently holds ~63K LOC of clean C across the subsystems:
`action.c`, `ani.c`, `battle.c`, `bind.c`, `cd.c`, `ev.c`, `fight.c`,
`fnmap.c`, `game.c`, `gfx.c`, `menu.c`, `resolve.c`, `save.c`, `vm.c`.

---

## 2. MANDATORY: ALWAYS SPAWN SUBAGENTS IN PARALLEL

**Every RE task is parallelized across subagents. Do not serialize work
that can run concurrently.** The user has explicitly required this — it is
the default, not an optimization.

### How to orchestrate

- **Partition by disjoint scope** so subagents never write the same file:
  one subagent per subsystem file (`cd.c`, `battle.c`, `save.c`, `ev.c`,
  `vm.c`, `gfx.c`, `menu.c`, `ani.c`, `action.c`, `fight.c`), or by guest
  address range, or by task type ("port clean" vs "runtime-verify").
- **Launch them in parallel** (`run_subagent` with `is_background=true`),
  then collect results. Keep scopes non-overlapping.
- **Each subagent prompt must be self-contained** — they cannot see this
  conversation. Always include:
  - the exact guest address(es) / function(s) to port,
  - the target `game/*.c` file and the naming convention (`renames.map`),
  - the build command and the headless debug command,
  - the delegation rule (§4) and the no-stub rule (§5),
  - that `generated/` is read-only reference, and
  - that they must verify at runtime, not just compile.

### Suggested parallel workstreams (pick per session)

1. **Port-clean sweeps** — one subagent per `game/*.c` subsystem, converting
   remaining `FUN_`/`LAB_`/`DAT_` bodies to faithful clean C.
2. **Runtime-verification agents** — one per subsystem, driving the headless
   debug server to prove the ported path actually executes (frame advance,
   correct MMIO, IRQ delivery, no livelock).
3. **Debugger/diagnosis agents** — one per active stall (e.g. the current
   CD seek-complete wait), tracing the exact guest path with the debug
   commands in §6.

---

## 3. THE CLEAN-PORT WORKFLOW (the established pattern)

For each guest function you port:

1. **Find the guest address** — `game/renames.map` and `game/fnmap.c` map
   addresses ↔ names. `generated/SLES_017.22_full_*.c` has the raw body for
   reference (read-only).
2. **Write clean C** in the right `game/*.c` file. Use a real name, real
   types, real control flow. Match surrounding conventions (the file already
   uses `guest_at`, `DB32`, `br2_st32`, `call_guest`, `bios_svc`, etc.).
3. **If the body can't yet be faithful** — a blocking wait, live guest `sp`,
   IRQ-timing, or deferred-coroutine interaction — **delegate the whole body
   to generated** instead of guessing (see §4).
4. **Regenerate the dispatch table:**
   ```
   python3 game/gen_clean_table.py     # rewrites src/clean_table.c
   ```
   It auto-skips any address whose clean body calls `br2_call_generated`,
   so generated callers dispatch straight to the generated body (no
   wrap→clean→generated recursion).
5. **Build:**
   ```
   cd build && cmake --build . -j8
   ```
6. **Verify at runtime** (§6) — compile-clean is NOT proof. The game must
   actually run the path.

---

## 4. THE DELEGATION RULE (why `br2_call_generated` exists)

When clean C cannot yet reproduce the guest semantics faithfully, the
correct move is **not** a guessed port — it is:

```c
uint32_t my_fn(uint32_t a0, uint32_t a1)
{
    return br2_call_generated(0x<guest_addr>, a0, a1, 0, 0);
}
```

This is already the established pattern for the hard cases:
`cd_init_session`, `cd_cmd_issue`, `cd_result_wait_a/b`, `cd_read_open`,
`cd_file_load_sm`, `cd_file_read_seq`, `cd_seek_complete_poll`,
`cd_mode_apply_deferred`, …

**Why it works:** the generated body runs real MIPS→C with
`psx_check_interrupts_at` at every basic-block boundary and a live guest
resume PC. Clean-C `while`+`br2_irq_poll` loops cannot supply that resume
PC, so the **deferred-interrupt coroutine starves** (CD `cd_ctx_restore→
cd_chan_full_init→cd_intc_dispatch`, kernel threads, etc.) and BIOS event
flags never get delivered → infinite `TestEvent` spins. Delegation is the
faithful choice until the clean port can express the same per-block
interrupt/guest-state behavior.

A delegated function is **not finished** — it is "generated-authoritative,
clean wrapper pending". Track it for later faithful porting.

> **⚠ UNSAFE PORT PATTERN — do not reconstruct guest CPS frames around BIOS
> services.** A clean port that rebuilds a whole guest function's prologue
> (DI/EI crit sections, `$ra`-cell saves, kernel-object setup) and then
> tail-calls BIOS services via `bios_svc6(...)` + `call_guest(0x80199fe0)`
> + a synthetic `br2_guest_ra_set(<jal-link>)` token is **NOT faithful** and
> has broken versus boot twice (commit `13576ce` reverted `cd_cmd_sequence`,
> `cd_kobj_init_a/b`, `cd_subsys_init_full`, `cd_mode_reset_seq`,
> `cd_mode_apply_seq`, `cd_chan_init_trio`). The generated CPS frame supplies
> a live `$ra` caller-token **and** live `a0–a3` forwarding that clean C
> cannot reproduce; OpenBIOS kernel-object/event services (`B0/0x57`,
> `B0/0x15`, `B0/0x12`, event flags) reject or misbehave → deterministic
> `FAIL-FAST unknown dispatch` or a `TestEvent` livelock. **Rule:** a port is
> safe only if it is (a) pure guest logic with no BIOS/kernel boundary, or
> (b) a *thin* trampoline that forwards straight to `bios_svc6` exactly like
> the guest `FUN_…` trampoline does (see `game/bind.c` `FUN_8019b380` =
> `bios_svc6(0xb0,0x08,…)`, and `bind_bios_call_live` which manages
> `cpu->gpr[31]` per-jal). Anything that *sequences* BIOS kernel calls inside
> a reconstructed guest frame stays `br2_call_generated`.

---

## 5. NO STUBS — the constitution applies here too

- Never `return 0;`, `return 1;`, `// TODO`, `// FIXME`, `// for now`.
- A function is either faithful clean C or an explicit
  `br2_call_generated` delegation. Nothing in between.
- Never write a "host pointer where a guest address is expected" port —
  `guest_at(x)` returns a **host arena pointer**, but generated callees and
  `cd_cmd_issue`-style functions treat the value as a **guest address**.
  Passing `guest_at(...)` into generated code reads garbage (this caused
  the `[0,0,0]` SetLoc livelock). Keep guest addresses as guest addresses
  end-to-end, or delegate.
- MMIO cells (`0x1F801xxx`, `0x1F8010C0`-family DMA regs) must go through
  `br2_ld*`/`br2_st*`/`br2_st32p`, never raw `guest_at` double-deref (the
  flat `0x1f800xxx` scratch mirror is not hardware).

---

## 6. RUNTIME VERIFICATION — the only proof

Compile-clean ≠ working. Verify against the real disc:

```
./build/BloodyRoar2_Recompiled \
    --disc /Users/tranquangdang21/Projects/bloody-roar-2/Game.cue \
    --debug-port 4370 --headless
```

Debug server `127.0.0.1:4370` (JSON lines). High-value commands:

- `frame`, `cdrom_state`, `cdrom_command_history`, `cdrom_trace_dump`,
  `cdrom_timing_dump` — CD path
- `bioscall_dump`, `bios_svc_trace` — BIOS syscall histogram (e.g. B0:11
  TestEvent spin vs B0:07 DeliverEvent)
- `mem_words {"addr":"0x801bcb24","count":8}` — guest RAM
- `sp_ring`, `dispatch_tail`, `fn_entry_tail`, `fn_stats` — call stacks /
  hot functions
- `irq_state`, `fntrace_arm`/`fntrace_dump`, `pc_probe_arm` — IRQ + PC probes

A port is **done** only when the game observably advances through it
(frames increase, correct sectors/data, no command storm, reaches the next
phase). "It compiled" and "it returns plausible values" are not done.

---

## 7. GIT

- Writable remote: **`mine`** → `github.com/quangdang46/BloodyRoar2Recomp.git`
  (`git push mine master`). `origin` is upstream reference — do not push.
- Commit real implementation changes; keep copyrighted disc assets out of
  the repo (see `ASSET_POLICY.md`).
- Match existing commit style: concise message focused on **why**.

---

## 8. CURRENT STATE & PRIORITIES

- `game/` clean tree is substantial (~63K LOC) but `generated/` is still
  load-bearing for many paths — **expanding clean coverage is the mission.**
- CD-ROM path delivers byte-exact Mode-2/XA data (right LBA/mode/sector slices
  verified vs `Game.bin`). **FMV/streaming wedge FIXED** (commit `be17669`,
  `proof/cd_dispatch_localization.md`): the tile/FMV stream and transient file
  loads SHARE the CD scratch arena `0x80010000`. `tile_stream_open`→
  `cd_dir_table_set` puts the 32-slot channel-entry table there; file loads
  (`cd_file_load_sm`→`cd_read_issue`→`cd_read_setup`→`bcb84`) DMA payload into
  the same arena and overwrite live descriptor `+4`/`+6` sequence tags →
  `cd_mc_state_machine`'s `u16@+6==u16@+4+1` gate never passes → producer
  starves → MDEC/FMV wedges. Root cause was NOT the arena sharing (intentional)
  nor delivery (byte-exact) — it was `cdrom_cycles_to_irq()` under-advertising
  the armed i_stat-bit2 presentation deadline to the fast chunker: it tested
  live `irq_flag`, but `present_cdrom_irq()` raises for `s_irq_i_stat_reason`
  while `s_irq_i_stat_armed`. A HINTSTS ack inside the 5000-cycle present
  window zeroed `irq_flag` without cancelling the owed presentation → chunks
  stepped up to `PSX_DEADLINE_HARD_CAP` (16384) cycles over the CD IRQ boundary
  → the deferred command-issue/stream-completion path missed its doorbell.
  Fix: deadline predicate now matches `present_cdrom_irq` exactly, plus
  due-now coverage (queued cmd / XA+CD-DA data-end / lid) and `cdda_delay`.
  Verified: `irq_present_max_late` 15539→0 cycles; fast instances clean past
  the ~frame-5556 wedge (75k+ frames) matching conservative control.
  `cd_response_pump` pc_probe "0 hits" was a FALSE NEGATIVE (clean-ported
  host C); `cd_result_wait_b` does the same callback dispatch synchronously.
- After CD stabilizes: continue clean-port sweeps across fighters,
  animation (`.ANI`/`.ACT`), the event/script VM, combat, input, memory
  card, and timing — until `generated/` is no longer required at runtime.

### Session update (arcade combat + delegation audit)

- **Arcade combat is REAL** (`proof/arcade_combat_verified.md`): P2 fighter
  ctrl (`0x1f8001f0`, stride `0x108`, `FC_FOE=+0x2c` cross-linked to P1
  `0x1f8000e8`) mutates every frame with ZERO pad-2 input → CPU-driven live
  fighter. `battle+0xb6==0` is the arcade single-player-vs-CPU phase
  encoding, NOT a missing opponent (versus uses the same ctrls/engine).
  Versus instance cycles real round phases (`b4b6` `00030003`→`00050014`→
  `00010011`). Earlier "no CPU-input synthesizer" proofs applied to the
  pre-fix hard-wedge, now resolved.
- **Bug fix `475e316`**: `player_name_entry` called `intro_movie_state()`
  with no arg; guest passes `a0=0`. Dropped param → returned 0 forever →
  `gs=9` wedge. Now name-entry progresses and arcade battles cycle
  `gs=8→9→5→8`.
- **Delegation audit** (`proof/delegation_wrapper_audit.md`): all 55
  whole-function `br2_call_generated` wrappers classified — 53/55 correctly
  MUST-DELEGATE per §4; only 2 borderline CD candidates (`cd_channel_table_
  init` `0x801A36F0`, `cd_dma_xfer_start` `0x8019b89c`), both complex MMIO/
  table fns that work correctly delegated. **Easy-wrapper surface exhausted.**
- **Real frontier** (corrected measure): the ~2586 raw `br2eu_func_*`
  generated entries are mostly CPS **split-fragments**, not logical fns.
  The true unported surface = **~101 `jal`-target fns** lacking clean_table
  coverage: ~55 correctly-delegated IRQ/CD/device fns + **~45 unnamed fns**
  (the real port targets). Easy unnamed leaves now ported; remaining are
  large (38-67 blk) or IRQ/CD-bound. Attack per-subsystem, NOT wrappers.
- **⚠ Background subagents stall on exec approvals** — `is_background=true`
  auto-denies tool approval, so build/launch/debug-server work hangs. Run
  port/verify sweeps as FOREGROUND (`is_background=false`) or do them
  directly. (Two bg agents produced zero output over many turns.)
- **Kill orphaned headless instances** — dozens of `BloodyRoar2_Recompiled`
  zombies (old `disc/` path, hours-old) starve the CPU and crawl build/
  verify cycles. Check `ps aux | grep BloodyRoar` and kill stale PIDs.

### Session update 2 (mode wedges SOLVED + versus combat restored)

- **Mode sweep** (`proof/non_arcade_mode_sweep.md`): ATTRACT demo + OPTIONS
  menu healthy. Corrected active-low pad map (old notes hit L1/L2/R1/R2):
  UP `0xFFEF` DOWN `0xFFBF` LEFT `0xFF7F` RIGHT `0xFFDF` SELECT `0xFFFE`.
- **WEDGE-A SOLVED `e9be0fe`**: `hud_state_machine` poll loop passed
  `frame_tick((intptr_t)guest_at(0x1f800000))` — a HOST pointer that
  `frame_tick` re-resolves via `guest_at()` → pad edge words wrote to a
  phantom slot → `afterdraw` gate never saw input → infinite poll. THE
  guest-vs-host pointer bug class (see §5). Fixed: pass guest const
  `0x1f800000`. NOTE: `pending {cmd:0x09,phase:1}` in cdrom_state is a
  COMPLETED Pause's stale residue (`active=0`), NOT a stuck command.
- **WEDGE-B / versus regression SOLVED `d20db90`**: `apply()` in action.c
  ran `.ACT` once-relocation BEFORE cursor-share; generated does it AFTER.
  Wrong order corrupted `field1` count → broke `ev5_action_select` gate →
  infinite `[EV5][EV2][EV1]` event loop → `gs=8 ph=8` freeze. Reordered to
  match generated. **Versus combat restored: 157k+ frames real fight**
  (was wedged ~frame 52k). `9c92faf` also added faithful `motion+0x84`
  next-pc mirror in ev_stream_dispatch.
- **Ports landed**: `cmd_queue_slot_next`, `scroll_offset_y_get`,
  `task_packet_emit`, `empty_leaf_e`, EV46 camera-op family (13 fns),
  `ev_stream_dispatch` +0x84. clean_table now ~1342 wrappers.
- **Remaining known issue**: mode `(1,4)` may still hit a separate CD
  `pending 0x09` starvation variant (`proof/cd_deferred_coroutine_
  starvation.md`) — distinct from the solved frame_tick wedge; needs its
  own look. Versus/arcade/attract/options are the verified-good modes.

### Session update 3 (coverage + faithfulness sweep — ROW5/ATTRACT fixed)

- **Mode sweep re-verified** (`proof/post_d11b51d_mode_verify.md`):
  VERSUS, ARCADE, row1, OPTIONS, ATTRACT all WORK on current master.
  ATTRACT reached 184k+ frames no freeze (f1d0894/800e54c foe-NULL +
  guard-removal fixes hold). ROW5 select now completes (records +0x24
  promote 0→1→2). **Only ROW3 `(1,4)` still wedges.**
- **Faithfulness bug class — defensive guards the guest lacks.** Clean
  ports added NULL/zero guards on guest pointers/values the generated MIPS
  dereferences unconditionally → divergence → freezes. `guest_at(x)` NEVER
  returns NULL (guest addr 0 = arena base = guest RAM[0]). Two sub-patterns:
  dead `if(guest_at()==NULL)` (benign) vs LIVE `x ? guest_at(x) : NULL` /
  `if(x==0)return` on a guest-address u32 the guest derefs anyway (BUG).
  Audited ani/cd/gfx/resolve/fight/game — all suspects verified FAITHFUL
  (guest libc string/memset fns DO have real arg null-checks; confirmed vs
  generated). The foe-NULL `.ACT` corruption (f1d0894) was the real instance.
- **Faithfulness bug class — MIPS `div` vs C `/`.** Guest `div` by zero
  yields LO=(dividend<0)?1:-1, NO trap; C `/` SIGFPEs. Added
  `mips_div_lo(num,den)` helper in `game/resolve.c` and applied at
  `keyframe_blend_apply` (@0x801859F8) + 8 `trig_*` sites (divisor
  `field24+4000`). Idiom elsewhere: `den!=0 ? n/den : (n>=0?-1:1)`.
  Verify each `/` site against generated before assuming it's a guest div.
- **ROW5 wedge FIXED `d11b51d`**: `fighter_spawn_setup` mode-3 had inverted
  compare — guest `beq +0x24==0 → merge`, clean had `!=0`. Pad-bit merge
  never ran → rec+0x24 never seeded → select deadlock.
- **ROW3 wedge — RESOLVED, was a REAL bug `b88ea82`** (supersedes the earlier
  "faithful, input-layer gap" verdict in `proof/hud_state_machine_row3_wedge.md`
  and `proof/row3_b6_phase_arm_trace.md` — those were WRONG): `hud_state_machine`
  `mode1body` (game/save.c) was missing the guest's unconditional `mode=2`
  delay-slot store at `0x8016F100` (`sw $t5,0x98($sp)`); clean wrote `mode=0`
  only on the `sp48!=0` path, so `sp48==0` left `mode==1` → infinite
  `modechk→mode1body` loop at tally `dat1=0x0b`. Fix: `mode=2` before the
  sp48 test → `mode2sel→fp=0xf0→fp=0xff→return`. Verified: tally cycles
  `03→04→08→0b→03`. See `proof/mode_1_4_resolution.md`.
- **Mode (1,4) match-end — FAITHFUL, no bug** (`proof/mode_1_4_matchend.md`,
  `8a219f5`): matches DO end via `case 11` (menu.c ~7177), NOT the
  `vs_confirm_c` tail gate (menu.c ~7888) which legitimately never runs in
  the CPU/demo path. rounds-to-win=2 (`MB8(MB32(0x1f8000c0)+4)`); full cycle
  1-1→2-2 draw→3-2→tiebreak→victory seq→results→next match. `gs=8` persisting
  = versus-attract demo loop, correct. **ALL 6 menu modes now verified WORKING.**
- **Ports landed `1d252aa`**: `cd_dir_scan` (0x80197E28 — replaces live
  call_guest on media-rescan path), `gte_rtps3_sxy_pack_ctrl` (0x80194F40
  GTE leaf), `op_tag_F_set`/`op_tag_K_set`/`op_node_rebucket` (0x801A
  task/record region). clean_table ~1347 wrappers.
- **Port frontier now small** (per `a587b873` manifest): remaining =
  0x801A GTE cluster (54F8/5698/5878/5A0C — lwc2/swC2 bulk loops),
  0x801A2808/0x801A2EC8 (jalr fn-ptr via 0x801BE79C), delegates
  0x8013C9CC/0x801587C0 (large CPS emitters), 0x801A0520/05E0 (BIOS
  trampolines — §4 risk, skip). Easy surface essentially exhausted.
- **⚠ subagent_explore can't see generated/** (gitignored) — its NULL-guard
  audit produced false-positive UNCERTAINs; only foreground/general agents
  or direct work can diff vs generated. Prefer foreground for anything
  needing generated verification.

**Bottom line: parallel subagents, clean named C in `game/`, generated only
as a temporary faithful fallback, runtime-verified, one unified source.**
