# Bucket A feasibility map — IRQ-boundary clean-port residual

Source: read-only feasibility agent d5e2a3e0 (full enumeration verified
against src/clean_table.c). Bucket B wiring (agent 397a3fdb) reduced
unwrapped to **58 = 57 Bucket A + impact_fx_update(0x80138cec)**.

## Blocker categorization (57 Bucket A fns)

| Cat | Blocker | ~Count | Examples |
|---|---|---|---|
| b/c | per-block `psx_check_interrupts_at` + CPS resume-PC / deferred-coroutine (SaveState/RestoreState) | ~27 | all read-path waits: cd_cmd_issue, cd_result_wait_a/b, cd_read_open/poll/setup, cd_init_session, cd_timer_read, cd_busy_wait, cd_seek_*, cd_chan_*, cd_queue_*, cd_response_process, cd_file_read_seq, vsync_wait_n, cd_file_load_sm |
| d | BIOS kernel-object/event boundary (B0 svc jr 0xB0; OpenEvent/TestEvent/DeliverEvent need live $ra caller-token + a0-a3 + DI/EI) | ~17 | cd_kobj_init_a/b, cd_subsys_init_full, cd_chan_init_trio, cd_channel_main_init/table_init, cd_cmd_sequence, cd_mode_*_seq/deferred, cd_intc_dispatch, cd_ext_init_once, cd_dma_chan_init, cd_hw_chan_init, cd_file_record_lookup |
| e | MMIO busy-wait / cycle-accurate hw polling | ~5 | cd_reg_stream_push, cd_reg_window_op, cd_reg_bit40_ctrl, cd_dma_xfer_start, dev_byte_xfer |
| a | live guest sp/frame clean ABI can't reproduce | ~4 | cd_mc_state_machine, cd_read_open, cd_read_step_cb, cd_mode_set_0e |
| f | indirect jalr via runtime-loaded guest fn tables | ~3 | op_cb1/cb2/cb4_store_ctx8 (save.c) |
| g | genuinely portable now | 1 | **cd_ctx_save 0x8019ad90** |

## What generated entry does that clean C can't express
1. CPS call splitting — every jal/jalr ends the C fn, sets gpr[31]=link,
   psx_check_interrupts_at(next), cpu->pc=target, return. Continuation
   blocks only re-run when dispatcher re-enters at that guest PC.
2. Per-block IRQ checks before every goto block_*.
3. Live guest register file + real guest stack frame (prologue sp-=N;
   sw s0-s7/ra/fp/gp).

psx_check_interrupts_at can longjmp into scheduler/exceptions resuming at
s_compiled_interrupt_resume_pc — a real mid-function block leader for
generated code. Clean C has NO mid-function resume point; clean_try_dispatch
runs whole wrapper then sets pc=gpr[31]. br2_irq_poll works for simple polls
(dev_cmd_commit, mc_op_result_poll) but starves the deferred CD coroutine
(cd_ctx_restore→cd_chan_full_init→cd_intc_dispatch→DeliverEvent) per
proof/cd_deferred_coroutine_starvation.md.

## Shared-mechanism assessment
- **(b/c) ~27 fns: NOT solvable by game/*.c idiom — needs runtime/recompiler
  CPS machinery.** Required primitive: suspend at a guest-visible PC and
  resume there after IRQ delivery. Conceivable via synthetic continuation
  addrs in a clean-side table (psx_is_dispatchable accepts any nonzero PC,
  psx_dispatch_game_compiled consults clean_try_dispatch first) — but that
  is a codegen/runtime project (per-block clean emitters + continuation
  dispatch + SaveState interplay), not a port sweep, and must interoperate
  with generated callees' CPS frames.
- **(d) ~17 fns: partial precedent, no shared fix.** bind.c has working
  hand-ports of the same pattern (FUN_8019a0ddc DI/EI + per-jal $ra tokens;
  FUN_8018b100 scratchpad + jalr loops). Each needs individual treatment +
  runtime proof; naive version (13576ce) broke boot twice.
- **(e) ~5 fns: re-audit candidates** — br2_ld*/br2_st* + br2_irq_poll +
  br2_guest_frame idioms postdate some delegation decisions.

## Quick wins (unwrapped 58 → ~52)
1. Add 0x801aed3c, 0x801aedbc, 0x801aef18 (op_cb*_store_ctx8, indirect-
   delegate) to WRAP_OK_DELEGATE — same proven-safe bypass argument; needs
   the "fp can never equal own addr" proof.
2. **Port cd_ctx_save 0x8019ad90** — generated body is a SINGLE basic block
   (full_46.c:31280, 60 bytes): 12 `sw` of live gpr[16..23,28,29,30,31] to
   [a0+0..44] + jr ra. Clean C reads cpu->gpr via clean_cpu_state()
   (precedent bind.c:1904-1911). Its delegation comment is outdated.
3. Re-audit cd_mode_set_0e, cd_dma_xfer_start, cd_reg_stream_push,
   cd_reg_window_op, cd_reg_bit40_ctrl, dev_byte_xfer — newer idioms may
   cover the cited mistranslation/cycle-granularity issues; needs runtime
   verification each.

## Honest bottom line
~48 of 57 are uniformly hard-boundary. The (b/c) majority needs CPS/
resume-PC machinery (recompiler/runtime work); the (d) minority needs
per-function BIOS-boundary ports with runtime proof. Neither is a
game/*.c sweep. generated/→zero for Bucket A is gated on building the
clean-CPS infrastructure — a dedicated runtime project, not a port pass.
