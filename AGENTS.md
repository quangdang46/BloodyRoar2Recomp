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
