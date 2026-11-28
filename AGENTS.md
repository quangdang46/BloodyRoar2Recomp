# AGENTS.md — Bloody Roar 2 clean-room RE mission

> Read `psxrecomp/CLAUDE.md` first — it is the faithful-core constitution
> (Architecture A locked, no interpreter, no stubs, LLE baseline). This file
> is the **project mission + workflow** that sits above it.
> Canonical objective docs: `docs/project/GOALS.md`, `ROADMAP.md`,
> `PROGRESS.md` — read them for the *what/why*; this file is the *how*.

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
- **§4 UNLOCKED for two classes (session 5, through `4d347c4`)** — the
  "EXHAUSTED" verdict above was too pessimistic. Two mechanisms are now
  PROVEN faithful and portable a real class of delegates:
  1. **Poll loops**: `while(cond){ if(br2_bail_pending())return <ret>;
     br2_irq_poll(); <guest stores> }` — `br2_irq_poll` (clean_bridge.c:308)
     advances 256 guest cycles + fires pending IRQs, so IRQ/device-deadline
     completion IS delivered. Ports: `cd_cmd_state_reset` (145a52d),
     `cd_cmd_issue_b`/`cd_cmd_issue_c` (753d03d).
  2. **Nested `jal`**: `call_guest4(fn,a0,a1,a2,a3)` / `call_guest` /
     `br2_call_generated` reproduce a guest `jal` (a0-a3 + `$t0=fn` +
     nested `psx_dispatch_call`; callee `jr $ra` returns to the boundary →
     back to clean caller). Same mechanism battle.c uses for prim_task_run
     jalr. Ports: `cd_timer_read` (8663946), `cd_busy_wait` (9091c92 — even
     reproduces a guest printf frame via br2_guest_frame).
  clean_table was 1356 wrappers after those two unlocks.
- **§4 UNLOCKED for a THIRD class + the cascade (session 6, through
  `08f68a2`)** — the unlocks compound: as callees get clean-ported, their
  callers' `jal` targets become all-clean → more delegates become portable.
  Two more mechanisms proven:
  3. **Live `a1-a3` `jal` forwarding — `cd_jal(link,fn)`** (cd.c:63, static;
     clone of bind.c `bind_jal`): sets `gpr[31]=link`, dispatches via
     `clean_try_dispatch`→`psx_dispatch_call`, does NOT restore arg regs
     (callee leftovers preserved like real MIPS `jal`), no `$t0` write
     (correct for `jalr $v0`). Solves the case where a `jal` must forward
     live caller registers the callee will consume. `call_guest`/`call_guest4`
     restore args — use `cd_jal` when the guest relies on leftover a1-a3.
  4. **Live `cpu->gpr[]` as DATA** — clean C CAN read/write the guest
     register file for register-file→memory ops (e.g. `cd_ctx_save` stores
     gpr[16-31] to a ctx struct). The "no guest register file" assumption was
     wrong; `clean_cpu_state()->gpr[N]` is live at the call boundary.
     Port: `cd_ctx_save` (ee1d892). The paired `cd_ctx_restore` is NOT
     portable (needs `jr $ra`→mid-fn PC via `psx_restore_state_escape`).
  **Cascade ports landed (~20 this session)**: cd_cmd_state_reset,
  cd_cmd_issue_b/c, cd_timer_read, cd_busy_wait, cd_ctx_save,
  cd_mode_set_0e, cd_reg_bit40_ctrl, mc_date_serial_cmp, cd_cmd_queue_load,
  cd_queue_push, cd_queue_drain, cd_reg_stream_push, cd_read_open,
  cd_read_param_set, cd_cb8_dispatch, cd_chan_init_trio, cd_reg_window_op,
  cd_mode_apply_deferred. clean_table now **1370 wrappers / 39 delegates**.
- **⚠ INSN-BY-INSN RE-DECODE before porting (session 6 lesson)**: two
  transcription bugs slipped past review and only runtime caught them —
  (a) `cd_cmd_queue_load` wrote `*cell = *cell2 + n*4 - 4` but guest computes
  `param_1 + param_2*4 - 4` (wild pointer → heap-PC dispatch 0x27AC);
  (b) `cd_queue_push` full-test is `(head+1)&0x3f==tail` not raw compare;
  (c) `cd_mode_apply_deferred` `0x10000-ent` not `-ent` (underflow → 4M
  iters). ALWAYS re-decode the generated block offsets/masks/shifts exactly.
- **Remaining ~39 delegates = the TRUE §4 hard class** (verified from every
  angle): BIOS/kernel-boundary ($ra-inspecting B0/C0 trampolines, e.g.
  cd_kobj_init_a/b, cd_dma_chan_init, cd_seek_step), fn-ptr `jalr`
  (op_cb*_store_ctx8, impact_fx_update), live `cpu->gpr` coroutine-resume
  (cd_ctx_restore, cd_read_poll, the 0x801BCBA4 done-flag wait family:
  cd_init_session/cd_read_setup/cd_read_step_cb/cd_result_wait_a/b/
  cd_cmd_issue/cd_response_process/pump/cd_file_load_sm/cd_file_read_seq/
  cd_read_issue/cd_mc_state_machine/cd_chan_full_init/cd_intc_dispatch/
  cd_chan_slot_set/cd_ext_init_once/cd_chan_cmd_pump/cd_hw_chan_init/
  cd_channel_main_init/cd_channel_table_init/cd_cmd_sequence/
  cd_subsys_init_full/vsync_wait_n/cd_mode_apply*/cd_seek_complete_poll/
  cd_dma_chan_init/dev_byte_xfer), and the big state machines
  (ret_zero_e, impact_fx_update). That is a runtime/recompiler capability
  gap (§4) — coroutine-ification or a scheduler-aware clean-call bridge —
  NOT incremental porting.
- **★ PROFILE-DRIVEN PORTING — the methodology that broke the delegate
  wall (session 6, through `98c0bc7`)**: static delegate-triage was
  exhausted, but profiling the fn_entry ring revealed the REAL hot
  generated surface is **unnamed per-frame game functions**, not the CD
  delegates. Recipe (reusable):
  1. Launch `PSX_FN_FILTER="0x80010000:0x80200000" ./build/BloodyRoar2_Recompiled
     --disc "disc/…cue" --debug-port P --headless --game game_1pad.toml`.
     `func_addr` is GUEST VIRTUAL (0x80xxxxxx), NOT physical — the
     `0x0000xxxx`/`0x1FCxxxxx` entries are kernel/BIOS (always-generated
     faithful core, ignore them; game range is 0x80010000–0x80200000).
  2. Drive gameplay: `python3 tools/versus_fight_mash.py P` (versus combat
     = heaviest per-frame coverage) or let attract play.
  3. `{"cmd":"fn_stats"}` then `{"cmd":"fn_entry_dump","seq_lo":S,"seq_hi":E,"count":N}`
     in chunks; aggregate `func` counts → hot-body histogram. Group
     `br2eu_psx_alias_body_*` entries (CPS aliases share one body).
  4. The ring holds last 262144 entries — kernel/BIOS IRQ noise floods it,
     so the game-range FN_FILTER is REQUIRED or you get 0 game hits.
  - Clean-ported fns are INVISIBLE to fn_entry (host C doesn't fire
    debug_server_log_call_entry) — so the histogram shows ONLY generated.
  - **Result**: ported ~50 hot unnamed fns this way — the whole
    `0x80187xxx`/`0x80188xxx` GTE `prim_emit_*`/`prim_cull*_emit` family,
    `prim_tri_batch_rtpt`, `desc_frag_*`, `op_dword_*`, `dev_byte_xfer`,
    `cd_seek_complete_poll`, `bios_tramp_b00a`, etc. Per-frame render/
    combat/game code is now CLEAN; the only hot generated left is the
    CD coroutine/BIOS/fn-ptr §4 class + `_start` boot artifact (2048×2
    `0x80126188/94` = BSS-clear loop, boot-only, ignore) + `0x801AA1E4`
    callback aliases (0x801AA244/2AC = mid-function alternate entries,
    need RECOMPILER alternate-entry support, not clean_table).
  - **Remaining generated hot = §4 CD coroutine**: cd_response_process
    (1372B/70blk), cd_response_pump (has jalr×2 + BIOS jals), cd_intc_
    dispatch, cd_read_step_cb, vsync_wait_n, cd_file_load_sm, cd_read_poll,
    cd_result_wait_a/b, cd_cmd_issue, cd_read_issue/setup, cd_file_read_seq.
    All confirmed coroutine-resume/BIOS-frame/fn-ptr — the §4 wall.
- **`cd_read_poll` PROVEN MUST-DELEGATE** (`4d347c4`,
  `proof/cd_read_poll_wedge.md`): faithful CFG port wedged the CD file-load
  path twice — the clean poll can't give the INT2 completion coroutine a
  live guest resume PC, so `cd_ctx_restore→cd_chan_full_init→cd_intc_dispatch`
  resumes against stale `cpu->gpr` → truncated host pointers at `0x801B563C`
  → tag-table load corrupts → `vsync_wait_n` spins at `0x8019A600`,
  `DAT_801bdde0` stalls. Needs a RUNTIME mechanism (re-enter generated
  dispatch for the IRQ window), not more clean-C restructuring.
- **The real "generated→zero" blocker is now NARROW**: not "all delegates"
  but specifically the deferred-coroutine-resume + BIOS-frame + fn-ptr-jalr
  subset. That is a runtime/recompiler capability gap (§4). Everything
  poll-shaped or simple-nested-call-shaped is now portable and largely ported.
- **§4 mechanism — precise wall (session 5 analysis)**: cooperative thread
  switch `psx_request_thread_switch` (traps.c:726) does
  `psx_save_context_to_tcb(current, gpr[31])` then `longjmp(g_scheduler_jmpbuf)`
  to `psx_scheduler_run` — it **abandons the ENTIRE host call stack**. The
  invariant (traps.c:721): "all guest thread context lives in CPUState/RAM/TCB
  — never in a host stack." So a clean-C `br2_irq_poll` loop CANNOT be the
  preempted thread: if its IRQ fires ChangeThread, the longjmp destroys the
  clean frame and the thread resumes at guest `$ra` in generated code, never
  returning to the clean fn. IRQ handlers DEFER the switch to the next dispatch
  boundary, so clean polls survive ONLY when completion needs no OTHER thread.
  `cd_read_poll` needs the CD INT2 coroutine thread to run → deadlock. The fix
  is coroutine-ification of clean code (or a scheduler-aware clean-call bridge)
  — a dedicated runtime-design effort, NOT incremental porting.
- **BIOS trampolines already done**: `cd_mode_apply`/`cd_mode_apply_wrap`
  (0x8019E400/410) are weak `br2_bios_tramp(0xb0,0x4a/0x4b)` forwards in
  bind.c (the §4-rule-b thin-trampoline pattern) — they show "unwrapped" in
  clean_table only because bind.c is excluded from gen_fnmap. True delegate
  count ≈ 51, all hard class.

### Session update (16c4e96 CD-port REGRESSION found + reverted `402925b`)

- **⚠ `16c4e96` faithfully ported 4 CD functions that WEDGE the game** —
  `cd_response_process` (0x80196370), `cd_response_pump` (0x801979f8),
  `cd_read_step_cb` (0x801985c4), `cd_intc_dispatch` (0x8019a8ec). Symptom:
  attract/boot stalls ~frame 15k — `cdrom_state.seq` freezes, `gs` stays 0,
  frames keep advancing (pending CD cmd `0x0A` phase-1 `active:0` starvation,
  the deferred-coroutine pattern). Bisected: `e8fd9ce`+`23dd2af` healthy to
  200k+ frames/3M+ seq; `16c4e96` wedges. **All four reverted to
  `br2_call_generated` delegates** — the response-path coroutine pair
  genuinely needs generated CPS live-resume-PC (§4), and the IRQ-dispatch/
  read-step ports are entangled in the same deferred-command activation.
  The agent's "verified 24k frames" claim was insufficient (didn't reach/
  survive the wedge mode). `vsync_wait_n` (23dd2af) port retained —
  verified healthy in the same run.
- **`cd_intc_dispatch` (0x8019a8ec) RE-CONFIRMED unsafe in isolation** —
  re-added alone (other 3 stayed delegated) and it wedged identically
  (seq froze ~5777, gs=0 ~frame 12k). The IRQ-dispatch itself needs the
  generated CPS per-block interrupt-check/resume contract — it is NOT
  safe even though its body is mechanical. Reverted; all four stay
  delegated. **Do NOT re-attempt any of these four (or cd_intc_dispatch_b)
  as clean ports** without a scheduler-aware clean-call bridge — the whole
  CD IRQ/response dispatch path is confirmed §4.
- **`cd_intc_dispatch_b` (0x8019a87c) port attempt ABANDONED** (was never
  committed healthy): first version wrote stage results to absolute
  `0x801c0014/0x801c0004` instead of `(*(0x801bdda0))+20/+4` (pointer
  deref) — corrupted CD state. Fixed version exists only in git stash;
  left generated-authoritative pending proper §4 review.
- **⚠ subagent_explore can't see generated/** (gitignored) — its NULL-guard
  audit produced false-positive UNCERTAINs; only foreground/general agents
  or direct work can diff vs generated. Prefer foreground for anything
  needing generated verification.

### Session update 7 (profile-driven porting round 2 — gameplay generated ≈ §4 CD only)

clean_table **1361 → 1461 wrappers**. Continued the profile→port loop; each
cycle re-profiled versus combat (`PSX_FN_FILTER` + `versus_fight_mash.py` +
`fn_entry_dump` histogram) and ported the new hot set. All pushed through
`0c4e7b8`. **The per-frame render/combat/game/op/scene code is now fully
clean C** — remaining generated gameplay traffic is ~90% the §4 CD coroutine
subsystem.

- **★ `fn-ptr jalr` IS portable via `call_guest4`** — corrected an over-
  broad §4 classification. `scene_node_walk` (`0x801B0DB8`, gfx.c:4497) was
  ~60% of ALL generated gameplay traffic (fn_entry ~20k/capture): a
  scene-graph dispatcher doing `type&0xFF`→handler-table→`jalr`. The jalr
  target is a dynamic guest addr — `call_guest4(handler,a0-a3)` handles it
  (saves gpr[31]/pc/v0, sets `BR2_NESTED_RA`, restores on `jr $ra`); the
  "continuation" is clean control flow, not a guest `$ra` jump. Multi-entry
  CPS families port as ONE clean body at the canonical entry.
- **Ports landed**: `scene_node_walk`, `gte_xform_packet` (0x801AAE08,
  ~2000 calls — GTE matrix/vertex transform-packet executor), `part_skin_
  emit` (0x801864DC), `prim_emit_{tri,quad}_{ot,fix}` + `prim_emit_{tri,
  quad}_tex_ot` + `gpu_draw_env_dma` (GTE polygon emitters), 18 BIOS kernel-
  vector trampolines + DI/EI + `cd_extent_len`, `elem_pair_field_set_alt`/
  `elem_flag1` (0x801AA244/2AC — mid-function ALIAS entries, BIOS CD
  sub-channel callbacks), 18 `op_*`/scene-handler leaves (0x801B0xxx +
  0x801ADxxx/0x801AFxxx).
- **New mechanisms**: `ARG_GPRS` in `gen_clean_table.py` (~line 353) marshals
  live `cpu->gpr[]` for functions entered via fn-ptr with non-standard
  register state (v0/v1 carry data) — e.g. `0x801aa244:(2,4,5,6,7)`.
- **★ `clean_trace` debug cmd + `CLEAN_RANGES`/`CLEAN_EXCLUDE` env vars** —
  THE verification probe for clean ports. fntrace/pc_probe show 0 for clean
  fns (they bypass `psx_dispatch_impl`); `clean_trace` records every
  `clean_try_dispatch` hit into a 512-ring. `CLEAN_RANGES` isolates a subset
  to clean while rest stays generated = ground-truth A/B.
- **Multi-entry check (learned hard way)**: before porting, grep
  `generated/SLES_017.22_dispatch.c` for MULTIPLE entry addrs → same
  `br2eu_func_*`. `gpu_img_boot_copy` (0x8019E5CC had entries E5CC/E0/F8)
  wedged at frame 195 when ported single-entry — reverted. CPS split
  fragments (cpu->pc= within one logical fn) are fine as one clean body;
  independent jal-targets to fragments are NOT.
- **§4-unsafe BIOS-frame pattern**: clean ports that rebuild a guest CPS
  prologue (DI/EI crit, $ra-cell saves) then sequence `bios_svc6`+`call_guest`
  tail-calls BREAK (cd_chan_signal_all, cd_ctx_cmd_submit_b/c reverted).
  Prefer `br2_call_generated` for BIOS-sequencing fns.
- **Final versus-combat fn_entry hot list** (all §4 CD coroutine):
  vsync_wait_n 0x8019A5D8, cd_intc_dispatch 0x8019A8EC/87C,
  cd_response_process 0x80196370, cd_response_pump 0x801979F8,
  cd_read_step_cb 0x80196718/85C4, cd_file_load_sm/read_poll/result_wait_*/
  cmd_issue/read_setup/file_read_seq + `_start` BSS artifact (0x80126188/94,
  boot-only) + tiny jalr stragglers (0x801421C4 battle, 0x8017DB88 event).

### Session update 8 (non-CD straggler sweep COMPLETE — only §4 CD remains)

- **16c4e96 CD-port regression confirmed + fixed `402925b`**: all 4 CD ports
  (cd_response_process/pump, cd_read_step_cb, cd_intc_dispatch) wedge the
  game ~frame 15k (pending cmd 0x0A starvation). Bisected: `cd_intc_dispatch`
  ALONE also wedges — the IRQ-dispatch itself needs generated CPS
  interrupt-check/resume. ALL reverted to `br2_call_generated` delegates;
  do NOT re-attempt any of them (or cd_intc_dispatch_b) as clean ports
  without a scheduler-aware clean-call bridge. `vsync_wait_n` (23dd2af) is
  the ONE retained CD-adjacent port — it polls IRQ-updated `*0x801BDDE0`
  VBlank counter via `br2_irq_poll` (safe: IRQ-handler-updated flags are
  portable; coroutine-resume is not).
- **Non-CD straggler sweep — DONE** (clean_table 1462→1471 wrappers):
  - `op_*` leaf helpers 0x801ad964/96c (hword/dword clear, v0 passthrough),
    0x801ae65c/801ae68c (gp-deref s16 add) → `game/gfx.c`.
  - `byte2_probe_commit` 0x8017cf6c → `game/menu.c`.
  - `desc_cmd_interp` 0x8017db88 (~91-block descriptor command-stream
    interpreter, 3 jump-table dispatches) → `game/menu.c:13161`.
  - `ev24_fx_spawn` 0x801421c4 cluster (~2400B CPS fn covering
    0x801421C4/42A40/42B0C + alias bodies) → `game/ev.c:1653`.
  - `prim_poly_emit_6c`/`prim_poly_emit_88` 0x80188510/0x80188754 (GTE
    polygon emitters) → `game/gfx.c`.
- **★ `resolve.c` EV-opcode dispatch unlock**: `ev_stream_dispatch` opcode
  ≥24 now uses `call_guest4(h,…)` (clean-table-first + generated fallback)
  instead of `br2_call_generated` — ALL EV opcodes can now be clean-ported
  incrementally. This is the template for unlocking other fn-ptr dispatch
  tables.
- **CPS multi-function clusters**: big generated "functions" like
  0x801421C4 are ONE logical fn split across `br2eu_func_*`+`br2eu_psx_
  alias_body_*` entries sharing a frame. Port as ONE clean body with an
  `entry` param (`ev24_cluster(entry)`), register only real jal-target
  entries in renames.map. Interior CPS pc-switch cases are suspend
  continuations — no clean suspend points needed.
- **Verified**: every port runtime-verified (versus FIGHT + 172k-360k
  frames gs=8 cycling, CD seq climbing). Final versus fn_entry game-range
  hot set = ~85-90% §4 CD coroutine family + kernel/BIOS low-addr
  dispatches (0x2xxx runtime layer, not game code).

**Bottom line: parallel subagents, clean named C in `game/`, generated only
as a temporary faithful fallback, runtime-verified, one unified source.**

**generated→zero status: ALL tractable non-CD gameplay code is now clean C.
The only remaining load-bearing generated coverage is the §4 CD
IRQ/coroutine/response subsystem (~90% of versus generated traffic) plus
the kernel/BIOS runtime layer. Reaching generated→zero requires the
psxrecomp scheduler-aware clean-call bridge (coroutine-ification) — a
deliberate runtime-architecture project, not incremental porting.**

### Session update 9 (refined §4 mechanism model — for the future bridge)

Investigation this session refined WHY the CD ports wedge — it is NOT a
simple "clean C can't yield" limitation:

- **`psx_fiber_*` already preserves native stacks** — `psx_change_thread_fiber`
  (traps.c) → `psx_fiber_switch` suspends/resumes the WHOLE host call stack
  (ucontext on POSIX). A clean C frame on a BIOS thread's fiber survives a
  cooperative thread switch intact.
- **`br2_irq_poll` already does the full interrupt check** — it calls
  `psx_advance_cycles(256)` + `psx_check_interrupts_at(cpu, gpr[31])`, the
  SAME entry generated code uses at every CPS block boundary. So clean C
  CAN trigger IRQ delivery AND cooperative thread-switch/fiber-switch.
- **The real blocker is guest `resume_pc` tracking.** When a thread yields,
  `psx_save_context_to_tcb` stores a GUEST resume PC (TCB+128) that the
  scheduler uses to re-dispatch. Generated CPS code maintains `cpu->pc` per
  basic block so the resume PC is always a valid continuation. Clean C does
  not advance `cpu->pc` per-step — after a yield+resume, the guest-level
  continuation is stale/wrong even though the host stack survives. The
  deferred-CD-command doorbell needs that guest resume PC to re-enter the
  coroutine at the exact CPS block → starvation.
- **Implication for the bridge**: making clean C a coroutine citizen means
  tracking a guest-visible resume point per yield (a "clean CPS" — either a
  guest-PC sentinel the scheduler recognizes, or converting clean wait-loops
  into explicit guest-resumable states). This is the scoped runtime work.
- **Confirmed safe discriminator still holds**: functions that only POLL an
  IRQ-handler-updated flag and return (vsync_wait_n) port fine; functions
  that ARE the scheduled coroutine (cd_response_process/pump,
  cd_intc_dispatch, cd_read_step_cb, file-load chain, cd_mc_state_machine)
  do not — they must yield at guest-tracked points.
- **Cold tail confirmed cold**: remaining unwrapped `br2eu_func_*` (~2444)
  are CPS split-fragments + boot `_start` region (0x80126xxx) — not
  independently portable and not hot. Attract+versus profiles agree.

### Session update 10 (CPS-transform approach CHOSEN for §4 CD — user decision)

User selected **CPS-transform clean CD functions** (keep deterministic
scheduler; hand-write coroutines as guest-resumable CPS). Scoping findings:

- **Yield is implicit**: CD coroutines have NO explicit switch call — they
  yield inside `psx_check_interrupts_at` at every CPS block boundary when
  the scheduler picks a target (e.g. `cd_response_pump` 0x801979F8 has 14
  such checks, 0 explicit switches).
- **★ resume-PC registration is the crux**: generated code passes EACH
  block's own next-block guest addr to `psx_check_interrupts_at(cpu, pc)`
  as the resume PC; on yield the scheduler longjmps then re-dispatches at
  that guest addr. `br2_irq_poll` instead passes stale `cpu->gpr[31]` —
  WRONG for CPS. A clean CPS port must call
  `psx_advance_cycles` + `psx_check_interrupts_at(cpu, <next-block-guest-addr>)`
  directly, AND every yield-capable block-leader guest addr must be
  registered in clean_table → a clean continuation that resumes at the
  right block with all state read back from `cpu->gpr[]`/guest RAM (host
  C locals die on the longjmp — keep ALL mutable state guest-visible).
- **Pattern**: per function = clean dispatcher reading `cpu->pc` (or entry
  param) → `block_*` labels + `goto` (like `desc_cmd_interp`/`ev24_cluster`)
  + per-yield-point resume-addr registration. Consider ONE clean fn per
  function taking a resume-block selector, with renames.map entries for
  each block-leader addr → thin wrappers passing that selector.
- **Start with `cd_response_pump` 0x801979F8** (simplest — pump loop around
  cd_response_process) before the 1372B `cd_response_process` state machine.
- **Verify each**: versus FIGHT + attract must stay healthy; a wedge means
  a resume-addr missed a yield point → bisect.

### Session update 11 (CPS-transform PROVEN — cd_irq_init_seq clean + versus FIGHT)

The CPS-transform mechanism is **implemented and runtime-verified**.
`cd_irq_init_seq` (0x8019A87C) is now a clean CPS function and versus
reaches/sustains FIGHT (`gs=8`, frame 659k, CD `seq` climbing). Committed
`6a5a388`.

**The mechanism (gen_clean_table.py + clean_cps.map):**
- New `game/clean_cps.map` — `addr name` lines for CPS functions AND their
  continuation addrs (continuations aren't in fnmap.c, so this file is the
  only way to get wrappers for them). Each addr → its own `wrap_<addr>`.
- `CleanDispatchEnt` gained a 5th field `flags`; `CLEAN_ENT_CPS=1` set for
  every addr in clean_cps.map.
- `clean_try_dispatch`: `if (_cps) cpu->pc = addr;` BEFORE `e->fn(cpu)` —
  routes the continuation addr in (generated sets `cpu->pc=entry->resume_pc`
  the same way). On return: `if (_cps) { if (cpu->pc==addr) cpu->pc=gpr[31]; }`
  else unconditional `cpu->pc=gpr[31]` — so a clean CPS fn's deliberate
  `cpu->pc=callee; return` tail-dispatch survives, while a normal return
  still jr-ra's. Non-CPS entries are byte-identical (setting cpu->pc for all
  1471 wrappers broke boot — BIOS/kernel services read it).
- All nested-call helpers (`call_guest`, `call_guest4`, `br2_call_generated`,
  `cd_jal`, `bind_jal`) save/restore `cpu->pc` around dispatches, so
  `cpu->pc==addr` after return reliably means "no tail-dispatch".

**Clean CPS fn recipe (verified):**
```c
uint32_t my_cps(void) {
  CPUState *c = clean_cpu_state();
  switch (c->pc) {            /* cpu->pc = the addr we were dispatched at */
    case 0xADDR_CONT1: goto bCONT1; ...
    default: goto bENTRY;
  }
bENTRY: /* entry block */ ...
bCONT1: /* continuation block */ ...
  /* tail-call a callee: */
  c->gpr[31] = NEXT_CONT;                 /* jal link = continuation */
  psx_check_interrupts_at(c, CALLEE);     /* yield check, resume=callee  */
  c->pc = CALLEE; return c->gpr[2];       /* tail-dispatch -> next fn   */
}
```
- `psx_check_interrupts_at` needs `extern void ...(CPUState*,uint32_t)` —
  declared locally (it's in psxrecomp/runtime/src/interrupts.h).
- Tail-dispatch targets can be generated fns (they re-enter via dispatch) or
  other clean CPS fns (registered in clean_cps.map).
- **★ STORE COMPLETENESS IS LOAD-BEARING**: the generated body hides `sw`/`sh`
  stores inside `_pgx` instrumentation braces — a filtered read drops them.
  `cd_irq_init_seq`'s four stores write INTC/init results into the CD
  context block; omitting them starved pending CD commands → `gs=0` wedge.
  ALWAYS dump the RAW generated ops (no filter) and transliterate EVERY
  gpr write AND every memory store/load.
- State must live in `cpu->gpr[]`/guest RAM — host C locals die on the
  scheduler longjmp. Registers s0-s3 etc. are read live from `c->gpr[]`.

**Next targets** (biggest §4 hot set): `cd_response_pump` 0x801979F8 (14
yield pts), `cd_response_process` state machine, `cd_read_step_cb`,
file-load chain. Same recipe; register every yield-capable block-leader addr.

### Session update 12 (cd_response_pump clean CPS + ★ sp-ownership fix)

Second clean CPS coroutine landed and verified: `cd_response_pump`
(0x801979F8, INTC handler[2] @0x801bcd1c) — the hot CD deferred-work pump.
12 blocks, guest 0x28 frame, drives already-clean `cd_response_process`,
two runtime fn-ptr `jalr` handlers. Replaces its §4 delegate. Versus
reaches/sustains FIGHT (`gs=8`, frame 199k). Committed `6252c2f`.

**★ CPS dispatch bug fixed — coroutines must OWN gpr[29].**
`clean_try_dispatch` saved `gpr[29]` before every wrapper and restored it
after (`cpu->gpr[29]=_saved_sp`) to emulate a skipped MIPS prologue. For a
CPS coroutine that manages its own frame this is fatal: the entry's
`sp-=40` was undone on return, and the epilogue's `sp+=40` + frame-slot
reads (`[sp+16..32]` for s0-s3/ra) hit wrong addresses -> `jr $ra` to
garbage -> deterministic boot crash (PC=0, ~4.1M insns). `cd_irq_init_seq`
survived only because it's a leaf that never touches sp.
Fix in `gen_clean_table.py`: `_cps` entries skip BOTH the frame
decrement AND the sp restore — `gpr[29]` is guest state that persists
across yields like every other `gpr[]`. Ordinary clean fns unchanged.
**Rule for CPS ports: if the generated body has a stack frame
(`sp±N`/`sw s-reg`/`lw s-reg`), the wrapper MUST NOT touch gpr[29] — the
coroutine owns it. Verify the prologue/epilogue restore offsets match the
generated saves exactly.**

**Indirect `jalr` in CPS:** a fn-ptr call is `gpr[31]=continuation;
psx_check_interrupts_at(c, target); c->pc=target; return` where target =
the live `c->gpr[rs]` — same tail-dispatch as a fixed jal, callee returns
to the continuation block via `jr ra`.

### Session update 13 (mechanical CPS transformer built + fragment-isolation finding)

Built `game/gen_cps_clean.py` — a source-to-source transformer that converts a
generated `br2eu_func_*`/`br2eu_psx_alias_body_*` CPS body into a clean
`game/cps_clean.c` coroutine. The generated CPS code is already the right shape
(pc-switch prologue, `block_*` labels + `goto`, `psx_check_interrupts_at` yield
checks, `cpu->pc=X; return` tail-dispatches), so the transform is:

- keeps `cpu->gpr[]`/`cpu->pc`/`psx_check_interrupts_at`/`goto`/`case` verbatim,
- drops ALL cycle/icache/cosim/debug/PGXP instrumentation + `#ifdef` guards,
- unwraps `{ uint32_t _pgx* = ...; REAL_OP; PGXP_*(...); }` temporaries,
- emits `void cps_<addr>(void)` + `CPUState *cpu = clean_cpu_state();`.

**★ MMIO-safe memory ops (critical correctness fix):** do NOT map
`psx_cyc_load_*`/`cpu->write_*` to `br2_ld*`/`br2_st*`. `br2_ld32`→`guest_at`
aliases `0x1F801xxx` hardware regs into low main RAM (WRONG for CD/DMA regs).
Instead map `psx_cyc_load_word/half/byte(cpu,A,rt,c)` → `cpu->read_word/
read_half/read_byte(A)` and keep `cpu->write_word/half/byte` verbatim — these
are function pointers bound to the MMIO-aware `psx_*` accessors, correct for
BOTH main RAM and hardware. Verified: `cps_clean.c` compiles clean incl. the
70-block `cd_response_process`.

**★ FRAGMENT-ISOLATION FINDING (why bulk CPS transform is not yet safe):**
CPS "functions" are often FRAGMENTS of a larger multi-fragment coroutine, not
self-contained units. Experiment: transformed `cd_chan_full_init` (0x8019A814,
3 blocks) + registered its 3 addrs in clean_cps.map → **CD starvation wedge**
(seq stalled ~6138, pending cmd 0x09). Root cause: `cd_chan_full_init` is "the
resumable half" of the `cd_ctx_restore→cd_chan_full_init→cd_intc_dispatch`
chain — a segment of a bigger coroutine whose resume graph spans sibling
`br2eu_func_*` fragments. Reverting restored health (seq 2.0M+).

**Rule: only transform SELF-CONTAINED coroutine clusters** — a `br2eu_func_*`
whose CPS continuation `case` addrs AND internal resume graph close entirely
within itself (like `cd_irq_init_seq` 8-addr and `cd_response_pump` 12-addr,
both verified). A fragment that is entered/resumed as part of a parent's
coroutine must be transformed TOGETHER with the whole connected cluster, or
left generated. The named multi-block CD fns (`cd_chan_full_init`,
`cd_read_poll`, `cd_intc_dispatch`, `cd_cmd_issue`, `cd_mc_state_machine`,
`cd_response_process`, …) are almost all `br2_call_generated` SELF-DELEGATES
(§4 BIOS/kernel boundary) AND/OR coroutine fragments — they are NOT safe
isolated-transform targets.

**Target landscape (CD range 0x8018–0x801B):** ~461 uncovered CPS `br2eu_func_*`
— 296 tiny (1–4 blk), 133 medium (5–19), 32 large (20+). Most named entries are
§4 self-delegates; the rest are fragments. Next step is cluster-level analysis:
identify self-contained coroutine clusters (closed resume graph) before
transforming — do NOT transform individual fragments.

### Session update 14 (clean-CPS sweep — CD coroutines landed; math batch REVERTED)

The "fragment-isolation" worry above was **overstated**: the real discriminator
is not fragment-vs-whole but **WHO owns the resume graph**. `cd_chan_full_init`
starved because it is the *resumable half* of the `cd_ctx_save→cd_ctx_restore`
KERNEL coroutine — the kernel snapshots the live `cpu->gpr` file via B0/0x19 and
`cd_ctx_restore` reloads + re-enters with `v0!=0`, a DIFFERENT resume mechanism
than `psx_check_interrupts_at`. Normal jal'd CPS subroutines and INTC-handler
roots resume via `psx_check_interrupts_at` and transform cleanly. `cd_mc_file_cmd`
(108 blk, 43 yields) confirmed CD command coroutines are in the safe domain.

**Genuinely landed (cps_ replaces br2_call_generated DELEGATES or fills a
no-clean-def gap — clean_table 1491→1744):**
- `cd_response_process` (70 blk), `cd_mc_state_machine` (19 split pieces/91
  addrs), `cd_read_poll`, `cd_read_step_cb` — all were §4 delegates.
- `cd_mc_file_cmd` (108 blk) + `cd_status_raw`/`cd_mode_byte_read`/
  `cd_param_buf_get` — no clean def.

**★ REVERTED (commit c0c611e) — the math/renderer batch was a FALSE-POSITIVE:**
I transformed 72 "pure-generated" math/GTE/renderer fns (`fixed_point_divide`,
`vec3_step_toward*`, `rtps_*`, `gte_*`, `prim_*`, allocators, `op_wrap_*`…) but a
narrow def-regex missed non-`uint32_t` return types, so they looked uncovered.
They were ALL already hand-ported in `battle.c`/`gfx.c`/`resolve.c`/etc. Since
`clean_cps.map` OVERWRITES the fnmap entry per-addr, the `cps_` transliterations
SHADOWED 62 working clean impls — faithful but a readability regression. All
batch entries removed; hand-port routing restored (`wrap_8018d300`→
`fixed_point_divide` again).

**★ RULE — verify a fn truly lacks a clean def before transforming.** Defs use
many return types (`bool`, `ushort *`, `undefined2`, `uint16_t`…), not just
`uint32_t`/`void`. `clean_cps.map` shadows, never complements — registering a
cps_ addr for an already-covered fn orphans its clean impl.

**Transformer now handles:** `psx_lwl/lwr/swl/swr`→`br2_*` MMIO-safe inlines;
split-piece `br2eu_func_X(cpu)` fallthroughs→`cps_X()` + forward prototypes;
`psx_game_text_native_ok` stale-static guards dropped; **COP2 lwc2/swc2** —
`unwrap_pgx_group` records `_pgxa/_pgxv` block-temp exprs (resolving earlier
temps, `psx_cyc_lwc2_read`→`cpu->read_word`) and substitutes them into the
surviving `cpu->gte_data`/`gte_write_data`/`cpu->write_word`/
`gte_precision_store_word` real ops. `gte_*`/`psx_gte_*`/`psx_muldiv_*`/
`psx_mult_latency_*` kept verbatim (declared in `cpu_state.h`).

**SAFE transform domain (verified):** normal jal'd CPS subroutines +
INTC-handler roots resumed via `psx_check_interrupts_at`, incl. CD command
coroutines. **UNSAFE (stay delegated/generated):** `cd_ctx_save`/`cd_ctx_restore`
kernel coroutines + chain members (`cd_chan_full_init` 0x8019A814 — verified
starvation), BIOS-boundary (`jr 0xB0` svc), `cd_result_wait_a/b`,
`cd_init_session`, `cd_read_setup`, `cd_cmd_issue_sync`, `cd_intc_busy_read`,
`ev_*` event coroutines (IRQ/coroutine risk — not proven safe).

**How to port a new batch (CORRECTED):**
1. Candidates = fns whose clean def is a `br2_call_generated` DELEGATE, or that
   have NO clean def at all. Detect defs with a BROAD return-type regex — do NOT
   trust a `uint32_t|void`-only scan. Exclude `cd_ctx_*`/BIOS/`ev_*`/INTC.
2. Compute each fn's **fallthrough-closure piece set** (follow ONLY
   `br2eu_func_X(cpu)` same-fn split calls + internal `cpu->pc` split-dispatches,
   NOT jal subroutine calls — scope `gpr[31]` per basic block). Require CLOSED.
3. `python3 game/gen_cps_clean.py --file <all piece entry addrs>` →
   `game/cps_clean.c` (single accumulation point — pass ALL kept entries).
4. Append `addr cps_<entry>` for EVERY case addr of EVERY piece to
   `game/clean_cps.map`; `python3 game/gen_clean_table.py`; rebuild; verify.
5. **cmake `file(GLOB)` caches** — re-run cmake configure when adding .c files.

**Remaining surface:** the named functions are now essentially all either
hand-ported, correctly-delegated (§4), or the risky `ev_*`/`cd_ctx`/INTC set.
The ~400 unnamed CPS fragments are mostly split-pieces of covered functions —
lower value, higher analysis cost.

### Session update 15 (mass clean-CPS sweep — 100% piece-entry coverage)

The §4 "must-delegate" barrier FELL: delegates failed *hand-porting* (no live
resume PC / `$v0` handle / gpr snapshot) but the CPS transform preserves
`psx_check_interrupts_at`+resume_pc+exact gpr mechanically — so IRQ-wait,
BIOS `jr 0xB0` boundary, ctx-init, AND kernel gpr-snapshot coroutines ALL
transform correctly. Key: register EVERY case/resume addr so kernel
`cd_ctx_restore→resume_pc` lands on a `cps_` whose `_cont` switch re-enters
the right block.

**clean_table 1709 → 8953 wrappers; `cps_clean.c` = 1281 faithful CPS fns.**
Batches: CD delegates (~30), `impact_fx_update` cluster, 0x8017 (95),
0x8014–18 (144), 0x8012–13 core (589), 0x8019–0x801b boundary (385), and the
final 4 kernel coroutines (`cd_result_wait_a/b`, `cd_chan_full_init`,
`cd_intc_dispatch`).

**Piece-entry CPS coverage = 100%.** Sole uncovered entry: `cd_ext_init_once`
(0x8019b308) — correctly `br2_call_generated` delegate per §4. The 17k
"case addrs" are intra-function block labels (goto-reached), not dispatch
entries — generated fallback covers them faithfully regardless.

Transformer notes: `psx_ws_cull_sltiu`+`psx_restore_state_escape` externs in
FILE_HDR; covered-fallthrough targets emit `cpu->pc=addr;return` (NOT `cps_X()`
— they'd shadow clean defs); dedupe piece-closures before registering.

### Session update 16 (embedded entries + absolute-zero uncovered)

Pushed coverage to literal zero at the dispatch level:
- `cd_ext_init_once` (0x8019b308) → cps_ (BIOS boundary, proven safe)
- `cd_dma_chan_init` (0x8019b310) → `_cont` injection: it's an alternate
  jal-entry inside ext_init_once's piece (unconditional-init variant
  skipping the guard-load). Added `block_8019B310` label + case.
- `cd_seek_step` (0x8019d508) → new `cps_8019d508`: shared-body entry
  inside hand-ported `cd_seek_complete_poll`'s piece; faithful copy
  minus the flag-load preamble (seek_step uses caller gpr[2]).

**Verified zero uncovered:** 0 named addrs, 0 piece entries, 0 delegate
targets missing from clean_table (8973 wrappers). `generated/` is now
load-bearing ONLY for ~87 `br2_call_generated` sub-calls inside clean
fn bodies — deliberate §4 delegations (BIOS svc, MMIO sequences,
coroutine ops needing live guest state). Those need per-function §4
hand-analysis, not transformation.

### Session update 17 (ZERO br2_call_generated call sites — all clean-first)

Converted the entire `br2_call_generated` call surface in `game/*.c`:

- **`call_guest4` bail-parity fix** (`1a93b83`): it had NO `g_psx_call_bail`
  handling — would restore pc/ra/sp after a callee began a wild-flow unwind,
  clobbering the resume target (same wedge class `br2_call_generated`
  prevents). Added pre-dispatch drop + post-dispatch skip-restore; unified
  the two identical reg-restore blocks. Latent bug for existing jalr users.
- **35 literal sites → `call_guest4`** (`9e4efec`): cd/battle/bind/game/save.
- **New `br2_call_clean`** (`cb0c8d1`): clean-first variant of
  `br2_call_generated` with the IDENTICAL register/bail contract — saves/
  restores ra/sp/pc/s0-s3, leaves a0-a3 callee-clobbered (faithful jalr
  chaining for `c->gpr[5..7]` forwards), honors bail, sets `t0=fn` per
  `jalr $ra,$t0`. `call_guest4` was REJECTED for these: its arg-reg restore
  breaks the chaining contract.
- **37 dynamic jalr sites → `br2_call_clean`** (`cb0c8d1`): battle/bind/
  game/menu/save. These manually set `gpr[31]`=link PC and forward live
  arg regs — needed the preserved contract, not call_guest4.

**Result: `game/*.c` has ZERO `br2_call_generated` call sites.** Every
guest call now routes clean-first (`br2_call_clean`/`call_guest`/`call_guest4`/
`gpu_invoke`/`svc_call`) with generated only as identical fallback.

**Runtime proof (`dispatch_stats`):** `static_hits=4,135,916`,
`miss_total=0`, `miss_unique=0` — every game-code dispatch hits the clean
table; `generated/` game code is never reached. `clean_trace` shows game
fns (0x80126178/88) running clean when called from BIOS. The only
generated dispatches left are `0x1FC0xxxx`/`0xBFC0xxxx` = the recompiled
BIOS (phys≥0x1FC00000 = BIOS ROM), which is legitimately generated per
psxrecomp architecture (BIOS = recomp target #1). **Game-side `generated/`
is dead code at runtime.** Verified: 44k+ frames, zero errors.

### ⚠ TOOLING LIMITATION — `read_ram` is faithful; the game is WEDGED (CD starvation)

**CORRECTED twice:** (1) NOT an overlay/paging bug — `read_ram`,
`write_ram`, `mem_words`, `guest_at`, `cpu->write_word`, and
`psx_read/write_*` ALL share the SAME backing with identical phys mapping
(`guest_at`→`memory_get_ram_ptr()`, `cpu.write_word=psx_write_word`,
`psx_phys_addr`/`br2_phys` both 2MB; scratchpad `0x1F800000` is a separate
`scratchpad[]` array that `psx_read_byte` DOES resolve). (2) NOT "idle" —
the headless instance is **wedged in the documented CD deferred-coroutine /
BIOS-event starvation** (`proof/arcade_residual_wedge.md`,
`proof/cd_deferred_coroutine_starvation.md`): main thread TCB `0x00008FE0`
(= live `s0`) is parked in a BIOS event wait; `bioscall_dump` shows
**1.36M historical `B0/0x11 TestEvent`** calls (frozen — thread now asleep,
woken only by VBlank); `card_mgr_trace` shows 2.78M historical card calls;
`get_registers` frozen at `ra=0xBFC05E00`/`sp=0x801FFD68`; `gwatch` on
`0x801FF000`/`0x1F800000` captured ZERO writes; `clean_trace` shows only
BIOS-called memset pieces; scratchpad `gs`/`ph` never written; pad START
presses do not wake it. **Consequence:** versus/state tools can't confirm
combat/memcard headless because the game never progresses past the
starvation wedge unattended — a KNOWN hard frontier (§4 deferred-coroutine
IRQ granularity), not a `read_ram` bug and not benign idle. Do NOT "fix"
`read_ram`; it reads correctly. The real unblock is the deferred-coroutine
DeliverEvent path — see the wedge proofs. Write-path probes that don't
need idle-memory reads: `gwatch`, `fntrace`, `clean_trace`,
`bioscall_dump`, `cdrom_state`, `dispatch_stats`, `card_mgr_trace`.

### Session update 18 (readability pass — named GPRs in cps_clean.c)

Mechanical readability win on the transliteration bulk: all **65,600**
`cpu->gpr[N]` refs in `game/cps_clean.c` now use standard MIPS register
names via file-local `#define` aliases (`zero/at/v0/v1/a0-a3/t0-t9/s0-s7/
k0/k1/gp/sp/fp/ra`). Purely cosmetic — macros expand back to identical
`cpu->gpr[N]`, so semantics/dispatch are byte-identical. Verified: clean
build, boot healthy, 0 dispatch misses. Mirrored in `gen_cps_clean.py`
(GPR_NAMES + `_GPR_RE` in emit_fn + #define block in FILE_HDR) so future
regens emit named regs.

**Readability frontier status:** operand-level readability is now good —
`sw $zero,0($v0); addiu $v0,$v0,4; sltiu $at,$v0,$v1` reads as real MIPS.
What remains is STRUCTURAL: cps_clean.c is CPS-fragmented (each "function"
is dozens of 4-40B basic-block pieces chained by `cpu->pc` returns +
`psx_check_interrupts_at` yield points), so reconstructing whole readable
functions requires decompiling the piece-CFG back into coherent control
flow — the true hand-port frontier, large and per-function. The named
cps_-routed fns are almost all correctly-transliterated CD/kernel
coroutines (§4 — can't safely hand-port); math fns (angle_*,
keyframe_cursor_search) are already hand-ported. Memory accessors
(`cpu->read_word` etc.) intentionally NOT aliased — they don't map
cleanly to signed/unsigned MIPS load names.

### Session update 19 (boot wedge ROOT-CAUSED to cps_ transliterations)

**The "frames advance = healthy" assumption was masking a deterministic
early-boot wedge on current master.** Fresh instances park identically:
scratchpad `0x1F800000` all zeros (`gs`/`ph` never written), main thread
TCB `0x8FE0` in BIOS `TestEvent` wait (`ra=0xBFC05E00`), ~2.78M BIOS
memcard-manager calls (`0x1FC0957C`/`0x1FC091E0`/`0x1FC08360`) — the §4
deferred-coroutine DeliverEvent starvation class. `read_ram`/scratchpad
nonzero is the real health signal, NOT frame count.

**A/B isolation (committed diagnostics):** `PSX_NO_CLEAN=1` (all generated)
and `PSX_NO_CPS=1` (named clean ports, all `cps_`→generated) both boot to
title; default wedges. **=> the `cps_` transliterations in `cps_clean.c`/
`gen_cps_clean.py` contain the boot-wedging defect** — coverage was proven,
correctness was not.

**Commit bisect:** `c0c611e` boots, `e00e5c6` boots, `5edaca1` wedges =>
first-bad `5edaca1`; function-bisect within it isolated `cps_801a0660`
(`cd_chan_cmd_pump`). On master the culprit moved/evolved (later commits
added ~1200 more cps_ fns).

**Master narrowing + its limit:** `PSX_CPS_KEEP` region bisect bounds the
bug to "reachable downstream of `0x80126178`" (the BIOS-called memset at
`0xBFC06694` head of the `0x8012` init fallthrough chain) — but direct
`cps_Y()` fallthrough calls bypass `clean_try_dispatch`, so region keeps
do NOT isolate cleanly. `clean_trace` shows the last clean dispatches are
`0x80126178`/`0x80126188`; the buggy fn runs later via untracked
fallthrough. Verified faithful: `cd_chan_cmd_pump` (63 instrs) + the
`0x80126178-9c` memset cluster incl. GPR dest regs. `PSX_CPS_COST_MUL=300`
does NOT fix => not flat cycle under-charge.

**Next:** fallthrough-aware isolation (route `cps_Y()` through dispatch, or
physically delete map ranges + regen per candidate); arm `fntrace`/
`pc_probe` to catch the last game PC before the BIOS park. Full writeup:
`proof/cps_boot_wedge_master_localization.md`.

### Session update 20 (boot wedge SOLVED — two cooperating cps_ defects, `2b57002`)

The deterministic early-boot wedge is **fixed and verified** (gs cycles
3/5/6 title/attract, frame 141k+, no BIOS park). Two cooperating defects in
`cps_*` transliterations starved IRQ/device deadlines; BOTH were required:

1. **Piece-boundary collapse (all regions).** Split-piece/next-function
   fallthroughs were bare `cps_X()` direct C calls → a piece chain ran in ONE
   `clean_try_dispatch` → cost charged once, `cpu->pc` stale → per-piece cycle
   accounting collapsed. `gen_cps_clean.py` now emits `cpu->pc=<piece>;return`
   (generated tail-dispatch) for all 721 sites; `cps_clean.c` regenerated.
2. **Intra-piece timing loss (`0x8018`–`0x801B` CD/IRQ/kernel).** A `cps_`
   piece runs its whole block with a frozen cycle counter; MMIO/BIOS-boundary
   fns never see the interrupt that breaks their wait. Removed those 2976
   transliterations from `clean_cps.map` → they run **generated** (§4-faithful).
   Kept game-logic `0x8012`–`0x8017` + hand-ported coroutines (`cd_irq_init_seq`,
   `cd_response_pump`).

Also fixed: `gen_clean_table.py` self-delegate detection now scans ALL bridge
calls (`call_guest`/`call_guest4`/`br2_call_clean`/`psx_dispatch_call`), not
just `br2_call_generated` — the 27 CD/IRQ §4 delegates forwarding via
`call_guest4(<own addr>)` are skipped → generated, no wrap→self recursion.
`src/clean_table.c`: 6003 wrappers, 27 self-delegate skips.

**A/B proof** (`proof/cps_fallthrough_cycle_wedge.md`): restricted-map alone
wedges; yields alone (full map) wedges; BOTH together boot. The proper
long-term fix is real per-instruction cycle charging inside `cps_` bodies so
the CD/IRQ region can be transliterated safely; until then the region split
is the correct conservative boundary. **Versus/arcade/attract gameplay
RE-VERIFIED on the fixed build** (session 21 below) — CD-heavy paths now run
generated and hold up under real combat + streaming.

### Session update 21 (gameplay re-verified + mode (1,4) RESOLVED — no wedges remain)

Post-`2b57002` runtime sweep on current master (`proof/arcade_combat_verified.md`,
`proof/mode_1_4_resolved_2p.md`):

- **VERSUS** `(2,1)`: sustained `gs=8` combat 125s+, live fighter ctrl-block
  mutation — real fighting.
- **ARCADE**: `gs=9` fight phases cycling (`0904→0901→0900`).
- **ATTRACT**: demo active — `P1ani`/`P2ani` changing, heap churning = CD
  asset streaming through the generated CD path works.
- **MODE (1,4)** `(1,4)`: **RESOLVED — was never a wedge.** It is a **2P
  versus mode**; single-pad confirm leaves side1 `+0xb6` unarmed → parks at
  results tally `dat1=0b` = *faithful* wait-for-P2. `tools/row3_twopad.py`
  (both pads confirm) → continuous match loop ~100 matches/111s, `e4`
  climbing `00/00→03/02` each match. Match-end phase `p92=0x0e` fires
  correctly. Earlier "open question" answered: versus-attract loops forever
  by design. Match/mode-flow is clean-ported `menu.c`/`save.c`, unaffected
  by the cps_ change.

- **INTRO FMV / MDEC**: plays ~40s of continuous decode (`command
  0x32000880`, `decode_macroblocks`≈325/frame, `output_size` varying per
  frame, `dma_in/out_words` live) then reaches title — **no ~87%
  truncation stall**. The `cd_deferred_coroutine_starvation.md` unified
  cause is resolved by the same fix: CD on generated restores
  `psx_check_interrupts_at` + per-instruction cycle advance, so the
  deferred CD coroutine resumes and streaming/FMV completes.

**No known mode wedges remain.** All selectable modes boot and run.

### Session update 22 (readability campaign — ev.c / cps_clean band / resolve.c)

Clean-room readability passes on `orch/w4/e02` (all build-clean,
headless-verified):

- **`game/ev.c`** (`281f1fc`): Ghidra-isms renamed in the 4 remaining
  functions (`ev5_action_select`, `ev_spark_spawn`, `ev_cmd_dispatch`,
  `ev24_fx_spawn`) — `param_*`/`uVar*`/`iVar*`/`puVar*` → role names.
  `g_DAT_*`/`PTR_FUN_*` externs kept (shared symbols, bind.c-owned).
- **`game/cps_clean.c` band 0x80190000–0x80197fff** (`f15413f`,
  `e90d657`, `db91115`, `e0b95f6`): 231 dead duplicate dispatch lines
  removed; `lui`+`addiu` and `<<16`/`>>16` sign-extend idioms folded;
  all 54 band bodies carry semantic `note:` comments. The 10 large
  entangled bodies stay in CPS transliteration form — restructuring
  them safely needs the `gen_cps_clean.py` template, not hand edits.
- **`game/resolve.c`** (31 commits): complete readability pass —
  ~1900 mechanical identifiers renamed, all 39 functions doc'd with
  `FUN_<addr>` guest addresses (renames.map-sourced), field-offset
  map in header, consolidated real prototypes, compound-assignment
  idioms normalized, 39 dead `DAT_`/`_DAT_` aliases + 3 dead extern
  fns + 2 dead extern globals + 0 unused prototypes removed.
- **Real bug found & fixed** (`3e904b2`): `action_commit` steering
  gate had `*ctrl < '\0'` — always false under unsigned `byte`;
  guest `lb`/`bltz` sign test restored as `(*ctrl & 0x80) != 0`
  (file's own convention). Found via `-Wtautological-compare`.
- **Verification method that worked**: strict-warning sweeps
  (gcc `-Wall -Wextra -Wshadow -Wmissing-prototypes
  -Wdeclaration-after-statement -Wtautological-compare
  -Wtype-limits`, clang bug-catchers + `--analyze`) caught the only
  real defect; peer-commit mining surfaced the team idiom set
  (`x = x op y`→`op=`, `+1`→`++`, `FUN_<addr>` doc annotations,
  dead-decl sweeps). Final headless verify post-all-commits:
  frames 23,681→50,872, no crash.

**Convention notes for future passes**: guest addresses stay as
`LAB_*`/`DAT_*`/`FUN_*` on this branch (peer branches rename them
semantically — merge will reconcile); `DAT_X` vs `_DAT_X` spellings
are both live (`&DAT_X` = host ptr to cell, `_DAT_X` = value);
register-model temps (`s0`–`s6`, `m0`–`m2`, `r1`–`r6`) and
`goto LAB_*` structure are deliberate faithful-port artifacts.

### Session update 23 (uninit-local sweep COMPLETE + Ghidra-ism rename campaign)

- **Uninitialized-local sweep finished** (commits `51d8e99f`..`1e4a12ab`):
  strict `-Wconditional-uninitialized`/`-Wuninitialized` went 32+14 -> 0
  across all of `game/`. Several were REAL bugs, not cosmetics:
  `results_scoreboard` tally state lost across per-frame calls (now
  `br2_guest_frame(0x88)` slots); `prim_list_gte_invoke` merged two guest
  counters (`num_out`=$s3 vs `num_in`=$s6) truncating the outer list loop;
  `vec3_step_toward`/`_spd` result-pack high halves are genuinely stale
  guest stack (never written) — backed by `br2_guest_frame` halfword
  slots; `slot_height_proj at_val` round-trips the live GTE regfile.
- **Ghidra-ism identifier sweep COMPLETE** in game/*.c: `local_*`/`param_*`/
  `uVar*`/`iVar*`/`in_*`/`unaff_*` live uses are now zero outside
  provenance comments. Renames landed: `memcard_file_flow` stepper desc
  (`3f5921c3`), `select_screen_tick` (`6b449d99`), `results_scoreboard`
  tally (`a56fdb1f`), `battle_flow_state_machine` two step_desc blocks —
  same `local_*` names reused per block, renamed per-block to
  `desc_a_fNN`/`desc_b_fNN` offset names (`41cbcd25`),
  `player_name_entry` string slots (`0704e504`), battle.c `in_*`/`unaff_*`
  spill locals -> `cop2_idxN`/`sN_in`/`gp_in` (`31542ea2`).
- **Deliberately kept**: `in_stride`/`out_stride` (gfx.c) and `in_g`/`out_g`
  (bind.c) — meaningful in/out pairs, not opaque temporaries.
- **Coverage re-verified**: 0 dispatch functions lack clean_table
  coverage; `cd_irq_probe.c` (`gen_cps_clean.py --timing`, 1116 bodies)
  already carries the §4-faithful CD/IRQ transliterations.
- **Remaining frontier**: the ~10 large entangled `cps_` bodies in
  cps_clean.c (largest `cps_8012fce0` 4767 lines) need gen_cps_clean.py
  template work to restructure into named clean C.
- **Ghidra typedefs ELIMINATED** (commits `7f851184`..`84440f27`):
  `undefined`/`undefined1`/`undefined2`/`undefined4`/`undefined8`,
  `byte`, `ushort`, `uint`, `ulonglong`, `longlong`, `code`,
  `int32_t_ghidra` typedefs deleted from battle.c/menu.c/resolve.c/ev.c/
  gfx_emit_p04.c — all uses substituted with the real stdint types
  (byte->uint8_t in battle/resolve/ev, ->int8_t in menu/gfx_emit_p04 —
  per-file signedness preserved). ~4700 sites, pure alias substitution,
  build clean. `CONCAT11/12/22/44` macros kept (locally defined,
  functional). Two -Wshadow warnings remain (isqrt_refine inner `n`,
  menu.c `f1c`) — deliberate faithful-port temp reuse, not defects.
- **-Wmissing-prototype warnings are DELIBERATE debt**: save.c's
  `name()` extern decls (~line 315) are intentionally unprototyped —
  Ghidra defs disagree with call sites on arg counts/types; tightening
  needs a per-callsite ABI audit (in-file comment says do NOT "fix"
  without one). Do not auto-generate prototypes. Same caution likely
  applies to menu.c (19) / battle.c (2) hits.
- **REAL BUG fixed** (`fac04fd9`): save.c op_dbg_dump* passed
  `(const char*)guest_at(0x80126xxx)` as text_win_printf arg0 — a host
  pointer truncated into int32_t `sel` — where the guest jal sets
  a0 = the guest ADDRESS (verified vs br2eu_func_801ACF34). Callee
  dual-uses sel (out-of-range => fmt guest addr via guest_at(sel)),
  so host ptr decoded to garbage. Fixed all 16 sites + tightened the
  extern + padded missing a2/a3 with faithful stale gpr[6]/gpr[7].
  ABI-audit notes: arg-count audit showed mc_data_*/mc_read_queue are
  really 13-param (earlier '7' was regex truncation); ret_one(void)
  called with args and mc_state_init/save_blk_cmp32 edge sites are
  harmless guest-sloppiness; angle_pair_rotmat '4-arg' was a
  multiline-parse artifact. print_dispatch/hud_text_b/
  announce_param_block_init take HOST string pointers — guest_at()
  args there are CORRECT (callee guest_of()s them internally).
- Headless verify post-commits: frame 33,915+ advancing, no crash.
  master ~35 ahead of mine/master, unpushed.
- **Session 24 — ABI + frame-discipline audits (subagent-driven)**:
  - save.c unprototyped-extern ABI audit COMPLETE: 4 REAL LP64
    pointer-truncation bugs fixed (`4f11c496`) — rotmat2_build,
    rotmat_rtps_build, angle_vec_build, angle_pair_rotmat declared
    `extern int` but return `uint32_t*`; op_wrap_* fed truncated
    (void*) into clean_table guest_of() wraps -> corrupt v0.
    prim_group_build tightened too.
  - guest-frame audit (`1daa6a3c`): results_scoreboard bail exit
    LEAKED its 0x88 frame (no free/abandon) -> sp drift; 10 sites in
    cd.c/save.c used free() on bail exits (over-restores sp) -> all
    converted to br2_guest_frame_abandon(). RULE: on
    br2_bail_pending() exits inside a br2_guest_frame scope ALWAYS
    abandon(), never free(), never bare return.
  - Return-type audit (`7d0660da`): latent-trap decls tightened
    (guest_memcpy, prim_list_gte/gte_invoke returns, euler_rotmat,
    gte_op_a00428). task_node_alloc left uint32_t deliberately —
    returns guest-address-valued pointer. fnmap.c `extern void
    name(void)` block is auto-generated + address-taken only = safe.
  - battle.c vec3 out-buffers uint8_t[8]->uint32_t[2] (`d87999cc`),
    hitres u16 casts (`b19a0114`). Uninit strict sweep still clean.
  - text_win_printf arg0 bug (`fac04fd9`) verified vs generated
    br2eu_func_801ACF34; printf-family callers audited consistent.
- Headless verify: frames 40k->67k advancing post-fixes, no crash.
  master ~42 ahead of mine/master, unpushed.
- **cps_ structuring FEASIBILITY PROVEN** (analysis, no code yet):
  SCC analysis of the 10 largest cps_ bodies shows the intra-function
  CFG is essentially a DAG — cps_8012fce0 (355 blocks) has ZERO
  non-trivial SCCs, 8015d69c (332) zero, most others 0-4 tiny SCCs
  (max 7). "Loops" run through cpu->pc self-dispatch across
  invocations (the coroutine model — semantically REQUIRED, IRQ
  yield points), not intra-function gotos. So classic reducible-
  flow structuring (Phoenix-style if/else intervals) applies almost
  everywhere; only the dispatch exits + _cont resume switch must
  stay. Much more tractable than decompiler-grade CFG recovery.
- versus_fight_mash.py driving headless build for deep-path
  verification of the session-24 fixes (frame 300k+ and climbing).

### Session update 25 (cps_clean goto-fold + full regen)

- `fold_fallthrough_gotos()` added to `game/gen_cps_clean.py` and
  `cps_clean.c` regenerated wholesale (743 unique `cps_*` addrs —
  extract via `grep -oE '^void cps_[0-9a-f]+\(void\)' | sort -u`,
  protos+defs double-count otherwise). Removes `goto block_X;` when
  block_X is the next block_ label in layout order and only
  braces/else/comments intervene — 1503 no-op fallthrough gotos
  deleted. Conservative: any executable line between goto and next
  label keeps the goto.
- Regen also picked up `fix_label_decls` `;` insertion (539 -> 0
  -Wc23-extensions warnings) and canonical renames.map names in
  dispatch comments. Verified regen-vs-prior diff was comments/`;`/
  gotos ONLY — zero semantic changes.
- `fix_label_decls` now skips comment lines when scanning for a
  following declaration (7 residual label->decl sites).
- cps_clean.c gotos: 7119 -> 5620. Build clean, zero warnings on the
  file. Headless verified (frame_fingerprint advancing ~1300fps).
- Structuring path confirmed next: with fallthrough gotos gone, the
  remaining `goto block_` sites are the real CFG edges — Phoenix-style
  region analysis on the DAG is the next generator milestone.

### Session update 25b (fold rollout + pointer-cast audit)

- `fold_fallthrough_gotos` rolled out beyond cps_clean.c/cd_irq_probe.c:
  - cd_irq_probe.c regenerated --timing: 2743 gotos removed (`ad16bdf1`).
  - 29 ad-hoc transliteration files (w2_*/w3_*/w4_*/w5_*/gfx_*/recon_*/
    ev.c) post-processed with the same rule: 1092 gotos (`13b4626c`).
  - Second pass with comment/string-aware brace matching (col-0 `}`
    bare-block style in menu.c/w3_e06/w3_e08 truncated pass 1): +165
    (`a0fdec9a`). Fold is idempotent; remaining `goto block_` are all
    load-bearing forward edges.
- Structuring rescope: 84% of all 4550 cps_clean blocks are `_cont`
  resume entries (even cps_8012fce0: 354/355) — labels are intrinsic to
  the coroutine model, not accidental mess. Phoenix-style nesting can't
  remove them; true readability = reconstructing whole guest functions
  across piece boundaries (the per-function hand-port frontier).
- Warning audit (`3d379871`): **real bug** — confetti_burst_update
  else-branch did `*(uint16_t*)(guest_addr_int)` raw host derefs (3
  sites) -> wrapped in guest_at; generated uses psx_cyc_load_half.
  All -Wint-to-pointer/-Wpointer-to-int sites now audited: remaining
  are the verified guest-address-valued pointer convention
  (br2_gaddr round-trips guest addrs; (u32)(intptr_t) containers).
  -Wpointer-sign (~28) are pointee-signedness mismatches, benign.

### Session update 26 (unprototyped-extern ABI audit — 6 real bugs fixed)

- Background audit of zero-arg/unprototyped `extern` call sites across
  menu.c/game.c found 6 REAL-BUGS; all fixed in `e07d53ea`, build clean,
  headless verified (frames 17k->26k advancing).
- `cd_stream_mode_init` (menu.c): param byte was a HOST stack buffer
  passed as guest addr through cd_mode_flag_set->cd_cmd_issue (LP64
  truncation + wild guest pointer). Now br2_guest_frame(0x28) backing,
  byte written at fr+16 = guest sp+16 (disasm 0x80159CF0-0x80159CFC).
- `deferred_cmd_drain()` called with 0 args at frame_tick (game.c:1536),
  frame_present (game.c:1958), display_frame_present (menu.c:2243):
  callee reads a1 uninitialized on undispatched cmd classes -> spurious
  record clear + counter decrement. Now passes live
  clean_cpu_state()->gpr[4]/gpr[5] (stale-arg convention).
- `confirm_code_select(0x1f800000)` missing a1 at menu.c:755/2160:
  a1 feeds mc_confirm_dispatch when sel_mode not in {0,1}. Guest sets
  a1=$s2/$s3=0x1f8000e8 before jal (full_27@12722, full_31@3692/14444)
  -> pass 0x1f8000e8 literal; extern tightened to (uint32_t,uint32_t).
- Earlier same-session fixes already committed: cd_chan_attr_push
  (0x801f2650), sprite_desc_table_init(1), hit_chain/pending_hit
  guest_at(obj) wraps.
- Verification method that worked: map clean call site -> fnmap guest
  addr -> find `CPS jal -> 0xADDR` in generated/SLES_017.22_full_*.c ->
  read last cpu->gpr[5] write before jal for true a1.

### Session update 27 (ABI audit round 2 + structuring negative result)

- Audit round 2 (subagent, all 51 unprototyped externs x call sites):
  5 more REAL-BUGs fixed in `4a8d53fc` — cd_dir_read_wrap host-ptr-as-
  guest-addr + discarded return value, cd_stamp_to_sector uint32_t*
  arithmetic 4x overshoot, mc_data_* p11 host-ptr truncation (3 sites),
  menu.c FUN_8018b100 macro shadowing the real 6-arg CPS-frame wrapper
  (all args silently dropped!), battle.c task_alloc_wrap_c payload
  truncation. Build clean, versus-mash verified frames advancing.
- **cps_ structuring: NEGATIVE RESULT — approach dead.** Implemented
  `structure_regions()` (Phoenix-style single-predecessor inlining,
  SCC bail, pinned=_cont-case targets) in gen_cps_clean.py, ran full
  regen over all 743 cps_ + sampled 400/518 cpst_ addrs: **inlined=0
  everywhere**. Root cause measured: EVERY non-first block in EVERY
  multi-block function is a `_cont` switch case target — the CPS
  coroutine model calls psx_check_interrupts_at before every dispatch
  with resume=next-block, so every block is a pinned resume point.
  Label-inlining can never fire; the labels ARE the continuation
  table. Reverted the pass (dead code). Only cycles found were
  self-loops (`if(c) goto self` poll spins) — not multi-node SCCs.
- True remaining frontier for cps_ readability: per-function hand
  ports reconstructing whole guest functions across piece boundaries
  (the established clean-port workflow), not CFG structuring.

### Session update 28 (cpst_ unmask sweep + prototype tightening)

- **Rotmat family unmapped** (`ad893e5c`): the batch-24 timed-coverage
  sweep had routed 8 rotmat entries (euler_rotmat_unpack/neg/core,
  rotmat_rtps_build_half, rotmat2_pair_build_word,
  angle_vec_build_half, rotmat_half_euler_a/b) to cpst_ bodies even
  though complete clean defs existed — the deferred "peer ports win"
  question from proof/cd_irq_timing_cps.md batch-24 audit. Verified
  leaf-safe (no calls needing CPS resume, ~300cyc IRQ granularity
  bound, same exposure as already-clean GTE prim_* family) and that
  euler_rotmat_core @0x8018DC20's only entries are wrapper tail-jumps
  + DC18 fallthrough, all with t7=LUT preset — clean forward is
  semantically identical. Removed 62 clean_cps.map rows; cpst_
  bodies remain as dead code (batch-44 precedent).
- **scene_node_walk stays masked**: 47 _cont rows + guest call_guest
  handler dispatch = the coroutine/IRQ-resume class from
  proof/masked_function_override_audit.md — single clean C fn can't
  express mid-body resume. Same for all cd.c cpst_ entries (§4).
- **Prototype tightening** (`44a92a6a`): applied the audited safe list
  (~49 decls across save/menu/battle/game) matching def signatures;
  held 4 category-B () decls (ret_one/ret_stub_b/noop_fn/
  cd_subsys_reset_wrap — args provably dead, prototype would be a
  constraint violation). -Wmissing-prototype/-Wdeprecated-non-prototype
  on touched files now down to those 4 deliberate sites.
- **Two REAL BUGS caught by the tightening**:
  - dialog_sel_dispatch_wide case 6 called u16_counter_step_pad_a
    with 2 args — guest jal actually targets 0x80129B48 =
    u16_counter_step_pad_b(a0+0x1e,8). Wrong callee, wrong
    counter-wrap semantics. Fixed.
  - cd_result_wait_b_fwd dropped cd_result_wait_b's v0 (guest jr ra
    passthrough; save.c:546 consumes cd_rc). Now returns uint32_t.
- **Pointer-convention audit** (`6788b9a2`): 4 sites where a guest
  address loaded from guest RAM was cast to a pointer for a callee
  that derefs it as host ptr — pair_depth_set x2 (camera_track_shot),
  impact_fx_spawn x2 (spark_trail_task_update, LAB_80140510). All
  guest_at()-wrapped.
- Method note: `psx_slice_block` is PARKED OFF by default
  (g_psx_precise_slice=0) — cpst_ yield checks are no-ops unless
  PSX_PRECISE_SLICE=1; the live mechanism is psx_check_interrupts_at
  per-block IRQ delivery.

### Session update 29 (first whole-function cps_ hand ports)

- **cps_8013c9cc → `stage_env_task_spawn`** (`3e714244`, game/battle.c):
  ev24 env-FX task spawner. task_node_alloc(1,2,10,0x8013B72C,0,7200)
  then two 2496-byte regions of 48×52-byte GPU packet records via
  gpu_pkt_tag_0c3c/prim_group_build/fighter_chan_delta_accum/
  prim_attr_word_set/task_node_free (all already clean). 80 map rows
  removed.
- **cps_801468ec → `fighter_emit_state_run`** (`95c56376`,
  battle.c): 67-addr/284-instr fighter emit state machine — GTE CT0-7
  matrix load, mode normalize via jr table @0x801202CC {0,4,2,6,1,7},
  three bit-selected emit sections walking 4-byte record lists off
  FighterCtrl, jal waypoint stubs 0x80136F70/80/78 via new file-local
  `br2_waypoint_jal` (same contract as bind.c bind_jal: ra/pc/sp
  save-restore around clean_try_dispatch→psx_dispatch_call). menu.c
  call sites (63/929/932) repointed; bind.c FUN_801468ec body deleted.
  renames.map:322 + fnmap.c entry; 138 clean_cps.map rows removed.
- **★ self-delegate trap + fix**: a clean port that calls its OWN
  guest addr through ANY bridge (br2_call_generated/br2_call_clean/
  psx_dispatch_call/call_guest*) makes gen_clean_table.py skip the
  wrapper (recursion through clean_try_dispatch). The no-cpu fallback
  `if (!c)` in fighter_emit_state_run was provably dead — every bridge
  no-ops on NULL cpu (clean_cpu_state()==g_clean_cpu) — replaced with
  `return 0`. Rule: never self-address a bridge call in a promotable
  port; if a fallback is needed it must not name the own addr.
- **v0 subtlety handled**: on the "bit1 ran incl. 0x2000 leaf_c, bit2
  clear" path guest v0 = (s6>>2)&1 = 0 (overwritten @0x80146C24/C28),
  NOT the leaf_c result — port returns 0 there explicitly.
- Verification: gen_clean_table 10636 wrappers/0 skipped; build clean;
  headless versus_fight_mash frames advancing (36k+).
- Frontier: cps_801587c0 (46a/337i, callees hud_text_d/spr_attr_alloc_*
  clean), cps_80161630 (74a/337i, largest), cps_80126234 (guest main,
  2 blocks) — analysis agents spawned.

### Session update 30 (cps_ frontier: 801587c0 ported, 80161630 promoted)

- **cps_801587c0 → `hud_gauge_frame_emit`** (`c1e0eb25`+`de1dac85`,
  menu.c ~11615): 46-block synchronous HUD gauge/frame emitter —
  "L:"/"R:" labels (rodata @0x80120DC0/C4, verified in disc) +
  mode-selected sprite emit. variant bits[7:4] = layout
  {0x00 quad bar via spr_attr_alloc_a, 0x10/0x20 strips via
  spr_attr_alloc_c with box-edge patches}, bits[3:0] = color index
  into 5-entry table @0x80120DCC (live-in regs when >=5). Metrics
  m0..m3 = MB32(0x801F46F8/36E8/36E4/46F0)*t0>>15; t0 = 128/208/
  live-in-$t0 by mode. **Callee contract fix**: spr_attr_alloc_c
  guest v0 = record guest-addr on success / 0 on failure (was
  inverted 0/-1) — its only caller dereferences rec+14/+22.
- **★ exit-$v0 is load-bearing**: sole generated caller
  (0x8015DED0 site) does `beq v0,zero`. Port returns uint32_t with
  per-path scratch verified against CPS tail blocks: invalid mode
  ->32, alloc_c fail ->0, mode 0x10 -> untruncated rec+22 sum,
  mode 0x20 -> m3>>3, mode 0x00 -> 4th alloc_a return. RULE: for
  synchronous ports, trace v0 through EVERY exit path — the CPS
  `jr ra` leaves whatever scratch the last block computed.
- **cps_80161630 → ret_zero_e promotion** (`2573eae2`): the ~290-line
  bind.c port was already complete (all 21 callees clean). bind.c is
  skipped by gen_fnmap, so added `ret_zero_e_entry_0x80161630`
  (save.c:6008) — addr-encoded `_0x<8hex>$` name suffix gives fnmap
  an entry automatically; forwards a0, returns flag as $v0. All 74
  clean_cps.map rows retired (10589->10516 wrappers). Mid-fn addrs
  still resolve via generated dispatch if ever reached.
- **Pipeline order matters**: gen_fnmap.py BEFORE gen_clean_table.py
  (clean_table parses fnmap.c + real prototypes for arg marshalling —
  got the 5-arg/stack-arg5 ABI right automatically).
- Verification: both headless versus_fight_mash verified (frames
  advancing 70k+/300k+). 120 clean_cps.map rows retired total.
- Remaining unnamed-cps frontier: only prim_xform3_task_run
  (0x8018b100, 12a named) + flat-region 1-2-addr fragments (port as
  part of parent). The whole-function hand-port campaign for
  unnamed jal-targets is effectively COMPLETE.

### Session update 31 (shadow-row audit + camera-piece retirement)

- **battle_camera_step CPS pieces retired** (`561b4cfb`): the 355-block
  `cps_8012fce0` cluster + siblings (`cps_8012fb54/fb58/fcd0/fcd8/
  31f48/31f4c`) are NOT standalone functions — they are the continuation
  pieces of guest fn 0x8012FB10, already clean-ported as
  `battle_camera_step` (battle.c:1266). All 383 clean_cps.map rows
  removed; mid-fn resume addrs now fall back to the generated alias
  body (better per-block cycle accounting than frozen-cycle cps_).
  RULE: a `cps_*` symbol whose addr range sits inside an already-ported
  fnmap function's span is residual CPS coverage, not a port target —
  retire the rows, don't port the piece.
- **Shadow-row audit verdicts** (fnmap addr ∩ clean_cps.map LHS,
  RHS != fnmap symbol, ~65 rows): the map row SHADOWS fnmap, never
  complements. KEEP ~44 — all CD/IRQ `cpst_` rows whose fnmap bodies
  are call_guest4/br2_call_clean delegates (removing reintroduces the
  §4 wedge class); sprintf_core_engine + text_format fragments same.
  REMOVED 16 provably-dead resume PCs (pad_slot_walk x5,
  scriptop_wait_fragment x4, printf_core_piece x7) — fnmap
  self-delegates fall to generated, faithful. STILL OPEN: 3
  VERIFY-RUNTIME conflicts where fnmap and map-RHS are BOTH clean
  impls of the same guest fn — 0x80188200/0x80188308
  (prim_quad_emit vs prim_poly_emit_*a CPS twins in w4_e02.c) and
  0x80194470 (gte_screen_init_hi_entry vs _defer_mfhi). Need
  insn-level diff to pick authoritative.
- **cps_8015b190 feasibility** (menu stepper row/callback dispatch,
  175 blocks): FEASIBLE, moderate. Selector compare chain + four
  bounded jump tables (0x80121158/198/1D8/1F0, all <15/<6 bounds).
  All callees clean. Dual ABI: callers pass host desc ptr via
  fn_lookup OR guest addr via call_guest — use br2_gaddr() normalize
  (pattern: dialog_sel_dispatch save.c:3583). u16_counter_step_pad_a
  and mode_string_dispatch want GUEST addrs; print path prefers host
  print_dispatch_selfdesc. Return v0=0. Next port candidate.
- **cps_8015d69c DEFERRED** (save/media stepper, 332 blocks): feasible
  but high-risk — menu_sprite_load_sm -> cd_file_load_sm is a
  deliberately-delegated IRQ path; synchronous port needs
  br2_guest_frame + br2_bail_pending checks around the sprite-load/
  cd_timer_read retry loop or it reintroduces the starvation wedge.
  Also documented dormant in observed traces — low runtime payoff.
- clean_cps.map now ~10100 wrappers; session total ~520 rows retired.

### Session update 32 (cps_8015b190 ported + committed)

- **cps_8015b190 -> menu_stepper_row_dispatch** (`3345663e`): the 175-block
  menu stepper row/callback dispatcher is now a clean C port in
  game/save.c (~5798-5936, beside dialog_sel_dispatch_wide). Dual-ABI a0
  normalized via br2_gaddr; sel = a1 & 0xffff. Covers sel 1-17 counter
  steps (u16_counter_step_pad_a), 0x100+ print paths (varargs marshaled
  identically to wrap_8012cf64: a2/a3 = live gpr[6]/gpr[7], stack words
  sp+0x10..0x1c), four guest jump tables @0x80121158/198/1D8/1F0
  (bounds 15/6, verified against disc/SLES_017.22 EXE), sel 0x10e
  config-string path, sel-14 MMIO/scratchpad commit path. All paths
  return 0. All 175 clean_cps.map rows retired; renames.map:541;
  clean_table regenerated (wrap_8015b190 -> clean call, gpr[4]/gpr[5]
  args, gpr[2] v0). Headless versus_fight_mash verified frames
  33k->44k.
- **Cluster audit in flight**: hit_react_apply_cps (~165 rows,
  battle.c:3722 port already live — likely pure deletion),
  memcard_dialog_cps (~145 rows, save.c:320 memcard_tick port — delete
  after save/load dialog runtime pass), fighter_script_fsm_cps
  (~146 rows — HOLD pending battle.c:6031 audit + coverage check).
- **3 verify-runtime conflicts** under insn-level diff
  (0x80188200/0x80188308 prim twins, 0x80194470 gte_screen_init).
- **3 verify-runtime conflicts RESOLVED** (`517946b9`, `4156e34b`):
  insn-level diff proved the non-timing CPS transformer was silently
  dropping psx_swl/swr (keep_re whitelist) — prim_poly_emit_54a/5ca +
  prim_point_emit_48a CPS bodies lost their OT-link stores. Retired 21
  map rows + stale renames; fnmap now routes 0x80188200/0x80188308 to
  prim_quad_emit_54/5c and 0x80188434 to prim_emit_48 (previously
  unregistered). 0x80194470 gte_screen_init_defer_mfhi map row KEPT —
  CLEAN_ENT_CPS is required for the cpu->pc=0x80194478 fallthrough.
- **GENERATOR BUG FIXED** (`b82af604`): keep_re in
  gen_cps_clean.py unwrap_pgx_group now whitelists
  psx_(lwl|lwr|swl|swr); cps_clean.c regenerated restoring 152 dropped
  br2_swl/br2_swr stores (76+76). Sweep proved no other live CPS body
  affected: cd_irq_probe.c TIMING mode always emitted cpst_*, and no
  w*.c/recon CPS body covers a generated func containing psx_swl/swr
  (the only victims were the dead prim trio).
- **hit_react_apply_cps RETIRED** (`191559da`): full coordinated
  deletion — 165 map rows + 6 alt fnmap/renames + w3_e02.c body
  (~2900 lines). Clean port battle.c:3722 already covers all semantics.
- **8 _alt/_alias clusters RETIRED** (`ac607b52`): 729 map rows +
  16 renames + ~11k lines of w3 CPS bodies (battle_flow_state_machine_
  alt, player_name_entry_alt, hud_score_draw_alt, action_from_input_
  alias, ev_cmd_dispatch_alias, hud_announce_cps, cmd_input_parse_*,
  battle_bringup_a_*). All were residual scaffolding inside already-
  clean-ported fn spans; piece addrs now fall back to generated alias
  bodies. battle_slot_draw stays (deliberate IRQ-yield CPS port).
- clean_cps.map now ~7894 rows (was 8623 at session start);
  clean_table 9031 wrappers.

### Session update 33 (cps_8017caf4 hand port — fighter_motion_step)

- **cps_8017caf4 → fighter_motion_step** (`ec53156a`): hand port of the
  55-block CPS cluster @0x8017CAF4 into battle.c (~140 lines). Per-
  fighter motion integrator: mode = s8(param_h0>>8) selects 1 of 4 arms
  over the state record's halfword channels (+172/+176 bounds,
  +192/+200/+202/+204 channels, +258/+260/+262 counters, +214/+216
  velocities); shared commit tail runs the battle_reset_tail-style
  conditional reset (deferred_cmd_post + fighter_reset_fx + slot_apply_
  guest + 3 hit clears). Uses br2_ld8/16/32 + br2_st8/16 guest-access
  helpers; arm 2 faithfully models cpu->lo low-word muldiv semantics.
  55 map rows retired; headless versus_fight_mash verified (26k→65k).
- **Frontier rescanned**: ~700 cps_ clusters remain. Top-of-list audit
  (3 background agents) classifying the biggest clusters as RETIRE
  (residual pieces of already-clean ports, like battle_camera_step)
  vs PORT (genuine unported) vs KEEP (IRQ/coroutine). Notable: many
  cluster bases sit 0x80–0x600 bytes after a named renames.map entry —
  strong residual-piece smell.
- **cps_801393b8 scoping** (34 rows, 1148B): GTE-using FX sprite
  placement routine, SPLIT across ≥3 clusters (801393b8→80139834→
  80139d98/impact_fx_type_alias) with gte_execute/rtps coord transforms
  + signed-div-8/div-4 halfword smoothing. Multi-cluster port — NOT a
  quick win; defer unless audits clear the field.
- **0x80126xxx region**: ~200 tiny clusters (1–6 rows) — init/atexit
  trampolines + small system fns (bzero wrapper → tail-jump chains).
  Low per-port value; batch candidate only if a shared convention
  emerges.

### Session update 34 (hit_react_apply crash fix + 246-row retire batch + wedge bisect)

- **REAL BUG FIXED** (`b3317582`): `hit_react_apply` passed
  `(uint8_t*)(intptr_t)def` — a guest ctrl address — as the HOST
  motion pointer to `trig_lut_read`/`trig_vel_flat`/`trig_up`/`trig_diag`
  (resolve.c callers pass an already-translated host ptr). Host deref
  at literal guest addr `0x1f8xxxxx` → SIGSEGV `KERN_INVALID_ADDRESS
  at 0x1f8002a2` on the battle_intro_flow → fighter_pair_update →
  hit_react_apply path. Found via `PSX_NO_CPS=1` runtime. ALSO fixed
  earlier this session (same commit chain): `hitres0`/`hitres1`
  redeclared `short[4]` — decompiler split each 8-uint8 slot into
  2-short + scalar; index-2 writes smashed `fighter_pair_update`'s
  saved x28 → garbage a3 → same crash class.
- **246 rows retired** (`d8a82e03`): audit-verified RETIRE spans —
  script_stream_tick (23), LAB_80137acc+LAB_80137cd0 opcode-dispatch
  (45), camera_track_shot (26), intro_movie_state (46),
  slot_attr_packet_build (87), fighter_atk_dir head (19).
  **display_page_commit span (19 rows, cps_8015e8bc) KEPT** — audit
  flagged its entry piece carries a prologue (possible alternate call
  entry); zero-cost insurance pending runtime smoke.
- **Boot wedge bisected to 0x80194000–0x801A0000**: `PSX_CPS_KEEP`
  env filter (clean_table.c:~34651, ranges of `lo-hi` keep cps_ ONLY
  inside; generated for the rest; named ports never filtered).
  Results: keep-80190000-801A8000 → WEDGED (gs=0, state
  `0000000020621280`); keep-complement-of-80194000-801A0000 →
  progressed (`...a0401e80`); keep-80198000-801A0000 → all-zeros
  (AMBIGUOUS — log shows stall in early VECWR/kernel-init, likely
  sparse-cps config hits a different early path or needs longer than
  the 120s probe). Region contents: the CD-driver coroutine cluster —
  ~1291 rows, mostly `cpst_*` timing variants (cpst_8019dad0 alone 222,
  cpst_80199440/cdst_80196370/cdst_80196e14 ~50-70 each) + named
  cd_irq_init_seq/cd_response_pump/cd_kobj_*. **`PSX_NO_CPS=1` runs
  the full game on generated paths — every RETIRE verdict is safe by
  construction; use it to verify any future mass-retire before commit.**
  Next step for wedge: per-FUNCTION isolation inside the region —
  bisect by retiring map-row ranges for the big cpst_ entries
  (rebuild-per-test), or extend PSX_CPS_KEEP granularity.
- **Pending audit PORT candidates** (9d974fa9): task fn @0x8013f828
  (~46 rows, KEEP→PORT as one fn — live task callback, GTE);
  cps_8013dbd8 (25, ends task_packet_emit @0x8013e0f0, GTE);
  cps_8013ae1c (26, ends fx_record_alloc @0x8013b088, jump-table
  @0x80120158); cps_80134c30 + continuation pieces (port as one fn
  ending @0x80134e44); cps_8014c324 (20, ends resource_rebuild_b
  @0x8014c76c). KEEP (do not touch): cps_8017b8d4, cps_8017e4d4,
  cps_8017e6b0, cps_8013c5e0 (unported generated gaps).

### Session update 34b — WEDGE ROOT-CAUSED (region proven, removal puzzle open)

- **Wedge mechanism fully diagnosed**: all-cps boot wedges at gs=0 with
  an **IRQ storm** — `i_stat bit0` (VBLANK) pending forever,
  `vblank_deliver`≈6.8M vs `vblank_ack`=6 (handler never acks). CPU
  spins in `cd_cmd_issue` (0x80196E14, 191M clean hits) ↔
  `cd_result_wait_a` (0x801968CC, 31M) — a CD command returns v0=-2
  forever; take_pc/epc pinned at 0x80196E14, ra=0x80126220 (post-
  `jal guest_main` break). Region = the CD-driver CPS/cpst_
  transliteration cluster.
- **PROVEN FIX (runtime)**: `PSX_CPS_KEEP="0x80120000-0x80194000,
  0x801A8000-0x801C0000"` (generated for 0x80194000–0x801A8000) →
  boot reaches **gs=06**, VBlank ack≈deliver (no storm), CD active,
  state evolving, versus-mash compatible. Bug lives in cps_/cpst_
  transliterations inside **0x80194000–0x801A8000** (~80KB, 1904
  clean_cps.map rows / 239 fns; biggest: cpst_8019dad0 cd_chan_attr_
  push 222, cpst_8019d740 75, cpst_80196370 73, cpst_80199440 65,
  cpst_801a8410 65, cpst_80196e14 cd_cmd_issue 53).
- **⚠ REMOVAL ≠ FILTER puzzle (next session's first task)**: deleting
  the same 1904 rows from clean_cps.map + regen → build crashes to
  PC=0 in early boot (`execution completed`, ~4M insns), EVEN with
  PSX_CPS_KEEP set. KEEP-filtering the identical addresses works.
  clean_find is exact-match so both paths return 0 → generated — yet
  outcomes differ. Hypothesis: some region addresses are cps_-only
  piece/resume PCs with NO generated block entry; KEEP-filtered
  entries still exist so a *different* dispatch (e.g. fnmap/call_
  generated wrappers, or the cpu->pc self-dispatch resume path)
  handles them, while table-absent addresses hit a null path. Find
  the first PC=0 crash addr (clean_trace/dispatch_tail before exit)
  to identify the gap. Alternative: keep rows but force generated via
  a build-time flag mirroring KEEP semantics exactly.
- **Half-region tests inconclusive**: generating only
  0x80194000-0x80198000 or only 0x80198000-0x801A8000 both stall
  early (VECWR/VECGA kernel-init, ~30-line logs) — likely the
  cps_↔generated boundary itself needs BOTH halves consistent, or
  early-boot IRQ handlers in the kept half break. Only FULL-region
  generated is proven.
- **Named fns in region** (renames.map, NOT cps_ rows — unaffected
  by row removal): cd_intc_dispatch 0x8019A8EC, cd_chan_attr_push
  0x8019DAD0, cd_cmd_issue 0x80196E14, cd_result_wait_a 0x801968CC,
  chan_job_pump 0x801A0110, cd_queue_a_read 0x801A1148,
  cd_cb8_dispatch 0x8019AE68, bios_tramp_b017 0x8019AD60 — the CD
  driver proper. The buggy transliteration is probably one of the
  cpst_ timing variants of these.

### Session update 35 (wedge puzzle SOLVED + frame-1467 crash root-caused to cpst_8018cfdc)

- **Removal-vs-KEEP puzzle RESOLVED**: deleting the 1924 clean_cps.map rows
  in 0x80194000–0x801A8000 is dispatch-equivalent to the proven
  `PSX_CPS_KEEP` config — PROVIDED every fnmap-named addr the cps rows
  shadowed is force-generated. `gen_clean_table.py` gained a
  `GENERATED_ONLY` set (47 addrs = every deleted row that also had a
  renames.map entry; the original 22 was incomplete — cd_cmd_issue
  0x80196E14, cd_result_wait_a 0x801968cc etc. were missing and their
  flag-0 clean wrappers ran where generated CPS frames were required).
- **NEW deterministic crash found + fixed**: `DISPATCH FATAL: misaligned
  target 0x00120201` at ~frame 1412–1467, ra=0x80126220 = the
  post-`jal guest_main` break — i.e. guest_main RETURNED via the
  bail-unwind path. **Pre-existing under PSX_CPS_KEEP** (old table +
  env reproduces identically); the earlier "proven fix" only fixed the
  gs=0 boot wedge, nobody ran it past ~frame 1412. Bisected with
  PSX_CPS_KEEP ranges (~3 min per probe): culprit = the
  **cpst_8018cfdc coroutine (cd_file_load_sm, 25 map rows)** — its clean
  CPS transliteration corrupts flow on file loads (~frame 1467 CD
  activity). `PSX_NO_CPS=1` never crashed (45k+ frames) confirming
  generated is authoritative for this fn — consistent with §4
  (cd_file_load_sm is already a delegated port calling
  `br2_call_clean(0x8018cfdc)`; with the rows gone it falls through to
  `psx_dispatch_call` generated).
- **Fix**: retired all 25 cpst_8018cfdc rows. Verified: boot healthy
  (gs=06, vblank ack≈deliver), 63k+ frames no crash, versus_fight_mash
  driving. NOTE: bisect configs were MIXED (entry piece clean + tail
  pieces ≥0x8018d000 generated) — every crash config had the entry clean;
  every clean config had all 25 generated, so the fully-clean coroutine
  is itself buggy, not just the boundary.
- **Watch for siblings**: other cpst_/cps_ auto-transliterations may
  have the same latent bug — whack-a-mole via the same KEEP-bisect if a
  new frame-N crash appears. `tools/bisect_keep.sh` +
  `tools/wedge_test.sh` are the probe harness.

### Session update 36 (ATTRACT-EXIT regression ROOT-CAUSED to guest_main clean port)

- **Symptom**: master @bbbd6966 boots to attract (gs=6) but NEVER exits —
  pad input dead everywhere (title/versus unreachable). Old commit
  d11b51d0 (pre-clean-port era) verified working.
- **A/B isolation**:
  - `PSX_NO_CLEAN=1` (all generated) → attract exits on START, reaches
    title gs=5 → mode menu → gs=8 versus flow. **Clean layer is the bug.**
  - `PSX_NO_CPS=1` (CPS transliterations off, named ports stay) → still
    stuck in attract. **CPS transliterations exonerated.**
- **Bisect** (`tools/bisect_clean_named.py` + `tools/attract_exit_test.py`;
  `CLEAN_EXCLUDE` + `PSX_NO_CPS=1`, ~2.5 min/probe): narrowed 1555 named
  clean entries → 51 addr clusters → single entry:
  **`0x80126234` = `guest_main` clean port** (wrap_80126234, committed
  cf3dea17). Excluding ONLY it (generated main) restores pad/attract.
- **Call-graph audit PASSED**: extracted every jal target from generated
  `br2eu_func_80126328` (the state-machine body the clean port merged)
  and matched all 7 states' call sequences 1:1 — no missing/extra calls.
  Entry chain 80126234→80126170→boot→loop is faithful.
- **Runtime evidence**: channel records @0x801ff080/170/260 (0xf0 stride)
  show runtime fields (+0x24,+0x34,+0x44,+0x54,+0x60) populated on the
  working build, zeroed on broken — pad/device channel poll starved.
  IRQ state identical both builds (vblank deliver≈ack, same masks);
  SIO tx_writes massive on both. Attr records 0x801befa8/ca stay template
  in BOTH builds during menu/attract — they are battle-path only, NOT
  the menu input source (menu reads `_g_pad_buffer` = scePadRead output).
- **Prime suspects** (unresolved): (a) IRQ/interrupt-check cadence —
  generated checks `psx_check_interrupts_at` at EVERY basic-block
  boundary; the merged clean mega-loop only polls via `br2_irq_poll`
  inside frame_tick (2x/frame) + per-dispatch entry checks — a
  sub-frame device deadline (SIO/channel timer) may starve;
  (b) guest-frame/sp discipline across the never-returning mega-loop;
  (c) a state byte the generated chain updates that clean doesn't.
- **Next**: instrument the channel-poll fn ptr (record+0x00 table,
  ~0x801d9xxx code ptrs seen in rec3) with pc_probe on both builds to
  confirm the poll never runs vs runs-and-finds-inactive records; then
  audit guest_main for the per-frame yield/BIOS-call boundary the
  generated chain hit that the clean loop misses.

### Session update 36b (guest_main bug narrowed to channel-slot ARM path)

Follow-up probes on the attract/pad regression (culprit = guest_main clean
port @0x80126234, proven by CLEAN_EXCLUDE bisect):

- **Channel queue service RUNS on broken build**: clean_stats shows
  cd_cmd_queue_service (0x801a189c) 440k hits, op_flag_fn_check
  (0x801a1834) 1.7M hits — the IntRP handlers dispatch fine as clean
  ports. pc_probe 0-counts were a false signal (clean dispatches don't
  hit generated block leaders).
- **Queue gates all pass**: BE7B8=1, BE7D4(1)>=BE7D0(0), table base
  BE7B4=0x801ff080 identical both builds; drain fns 0x801a1b50/0x801a1e84
  fire on both.
- **Slot records never ARM**: 0x801ff080/170 runtime fields (+0x24
  active=1, +0x2c, +0x34, +0x44, +0x54=0101010101, +0x60) populated on
  working, all-zero on broken. Slots hold ptrs to pad report bufs
  0x801befa8/ca at +0x30 (written by cd_channel_table_init — identical).
- **fn-ptr table @0x801c4784 DIFFERS between builds**:
  BROKEN=0x8003FD44, WORK=0x80047968 (both in low 0x8003-0x8004 region,
  not 0x801a game code — likely kernel/2nd-segment pointers whose value
  depends on init order/state). cd_channel_main_init does "two jalr
  calls through fn slot @0x801c4784 on channel bases 0x801ff080/+0xf0"
  — the slot ARM calls. Prime suspect: those jalr targets (or the
  table contents) diverge because guest_main's clean init path leaves
  different state when cd_channel_init_seq runs.
- **Caveat**: record diffs were captured WORK=title(gs5) vs
  BROKEN=attract(gs6) — partially state-confounded; the +0x24 active
  flag pattern is still the best lead. Attr records 0x801befa8/ca are
  battle-path only (template in menus on BOTH builds) — menu input is
  `_g_pad_buffer` (scePadRead output), a separate path also starved.
- **Next**: identify what writes 0x801c4784 (fn-ptr table install in
  cd_channel_table_init/cd_handler_*_install) and why the value differs;
  trace the two channel-init jalr calls in generated 0x801a1a40.

### Session update 36c (pad-path forensics — records vs pad_buf)

- cd_channel_main_init's two jalr calls go through fn ptr **0x801BE784**
  (=0x801a3850 `cd_cmd_rec_clear`, a record scrub — NOT the arm path;
  earlier 0x801c4784 lead was a different table, red herring). Fn table
  @0x801be780-0x801be79f is byte-identical on both builds.
- pad_buf located: `7f1c` signature at **0x801fffd0** both builds
  (pad_buf[1]=0x1c7f init). After a 60-frame START press the buffer
  shows NO button data on EITHER build — scePadRead either isn't the
  menu source or writes elsewhere/format differs.
- Attr records 0x801befa8/ca compared in SAME state (both gs=6
  attract): WORK=`00 41 ff ff` pattern, BROKEN=`ff 00 00 00` — real
  divergence in the pad-report records, written by the channel pad-job
  completion (SIO read → report buf). The poster of pad-channel jobs
  is still unidentified — channel_script_emit* family is battle-side
  (hit FX), not pad.
- SIO itself is healthy on broken (tx_writes millions, pad_buttons
  register on emulator side). The missing link is specifically the
  pad-channel JOB that fetches reports into 0x801befa8/ca.
- Open question for next session: which guest function posts the
  periodic pad-read job into the 0x801ff080 queue (likely a per-frame
  driver inside the match/attract path or an IntRP-driven coroutine),
  and why guest_main's clean loop prevents it.
