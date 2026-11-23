# 0x801BCBA0 wild-dispatch — static root cause (agent ba3a3c47)

Crash: `FAIL-FAST unknown dispatch addr=0x801BCBA0 ra=0x801A1EC0` at frame
49548, `in_exception=1`, `bail_flattened=8`, SIO/CD poll path.

## Verdict
`cpu->pc = 0x801BCBA0` is a **BIOS/OpenBIOS thread-context restore committing
a saved resume-PC/EPC**, ending in `jr $k0` where `k0 = *(TCB + 0x88)`. It is
**NOT** a legit saved resume-PC and **NOT** a handler-table/indirect-jalr
read. `0x801BCBA0` = `DAT_801bcba0`, the CD read-session sector/completion
counter (`game/cd.c:256-261`, written `cd.c:552`, documented
`proof/cd_read_done_event.md` `+0x20/0x801BCBA0`) — pure BSS data.

## Commit paths (traps.c)
- `psx_syscall` ChangeThread/RFE fallback: `cpu->pc = saved_epc`
  (`traps.c:1060-1077`)
- scheduler restore: `run_pc = psx_restore_context_from_tcb(...);
  cpu->pc = run_pc` (`traps.c:931-955`)

Register signature favors the **`psx_syscall` case-3 manual TCB restore**
(`traps.c:1065-1077`): it restores `gpr[26]` from the stale context slot
rather than loading `k0 = saved_epc`; dump `gpr[26]=0x00008FE8` matches that
path leaving k0 untouched.

## Context chain
```
*(0x108)      -> PCB/current-context pointer
*(PCB)        -> current TCB/context
TCB + 0x88    -> saved resume PC / EPC   (psx_save_context_to_tcb save+128)
```
Restore read `0x801BCBA0` from that slot → corrupted saved-PC / wrong
context selection. `psx_is_dispatchable()` only rejects 0 + sentinel, so the
nonzero data addr reaches `psx_unknown_dispatch` → FAIL-FAST.

## Leading mechanism
IRQ-dense SIO/CD path: either **wrong context block restored** (a transient
kernel-stack/CD deferred context treated as the architectural current TCB)
or **saved-PC slot clobbered by a data write**. Former slightly favored —
`0x801BCBA0` is a plausible data pointer that could sit in a transient
context. `defer_switch=false` already set (`game_1pad.toml`); this is a
residual/later corruption, not the old frame-~1600 shared-kernel-stack bug.

## Ruled out
- dev_cmd_round_dispatch table/index (counter=1, table valid)
- ra=0x801A1EC0 (stale $ra), last_store_pc=0x801A2160 (unrelated SIO store)
- The freeze dump lacks the decisive write: `thread_trace` empty, no wtrace
  entry for value `0x801BCBA0` — exact writer still unproven.

## Faithful fix direction (do NOT clamp 0x801BCBA0)
Correct context ownership/save timing:
1. `*(0x108) -> PCB[0] -> TCB` chain and host scheduler must agree at every
   RFE/ChangeThread boundary.
2. Nested IRQ / temporary kernel-stack / CD deferred contexts must not be
   treated as the architectural current TCB.
3. Preserve interrupted thread's real EPC/GPRs across nested IRQ delivery in
   clean→generated→BIOS call chains.
4. Audit the `g_psx_call_bail` unwind (`bail_flattened=8`): prior proof
   suspects a clean callee leaking guest `$sp`/`$ra` in
   `cd_cmd_queue_load`/`dev_cmd_round_dispatch`
   (`proof/gs5_dat1_3_bail_exit_invariant.md`).

## Next instrumentation (first bad restore)
Latch: `*0x108`, `**(0x108)`, resolved TCB base; `TCB+0x88` value + last
writer PC; whether resolved ctx is a real TCB (`0x8Fxx`) vs transient
stack/CD ctx; `g_exception_real_epc`, `take_pc`, `entry_tcb`, `exit_tcb`;
any write of `0x801BCBA0` into RAM incl. host `cpu->write_word` paths wtrace
may miss.
