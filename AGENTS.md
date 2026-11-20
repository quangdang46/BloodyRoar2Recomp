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
- CD-ROM path is *nearly* working: file loads deliver real Mode-2/XA data at
  correct MSF/LBA, mode `0xA0`, reads complete. Active blocker: the next
  file's seek never reaches the device (deferred command-issue / register-
  window doorbell in `cd_seek_param_issue`→`cd_reg_window_op`→worker).
- After CD stabilizes: continue clean-port sweeps across fighters,
  animation (`.ANI`/`.ACT`), the event/script VM, combat, input, memory
  card, and timing — until `generated/` is no longer required at runtime.

**Bottom line: parallel subagents, clean named C in `game/`, generated only
as a temporary faithful fallback, runtime-verified, one unified source.**
