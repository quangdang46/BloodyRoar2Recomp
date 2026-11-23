# CD coroutine stack — mechanism VERIFIED working; file-load path still open

## What the oracle (DuckStation, port 4371) actually showed

Continuous `get_registers` sampling (~6.29M samples) found min sp=0x801BDC08
at pc=0x8019A4A8 (cd_timer_read region), ra=0x80196B88 — the 0x801BDxxx
coroutine stack. Main-stack min was only 0x801FFE80 (shallow). rec_bad=0:
the device-command record rec+0x14 never held a session pointer on HW.

**CAVEAT (corrected):** the oracle NEVER reached cd_file_load_sm
(pc_break hits=0 — it stayed in attract/menu CD polling). The 0x801BDC08
sample is the ROUTINE per-frame CD-IRQ coroutine dispatch
(cd_ctx_restore->cd_intc_dispatch), NOT a file-load wait chain. So this is
NOT yet proof of where HW runs the file-load wait chain.

## What OUR runtime shows (spdom ring, port 9796)

The coroutine stack-switch WORKS identically to HW — every frame:
  0x801FF398 -> 0x00008548  (main -> BIOS IRQ stack, target 0x00002924)
  0x00008548 -> 0x801BDD30  (IRQ -> coroutine, target 0x8019A87C = jmp-buf ra)
  0x801BDD30 -> 0x801FF398  (coroutine -> main yield, target 0x8019A53C)
The jmp-buf at 0x801BCD50 is armed {ra=0x8019A87C, sp=0x801BDD30}.

## The actual crash vector (spdepth ring)

During file loads the MAIN thread (tcb=0x8FE0) runs the synchronous wait
chain cd_file_load_sm(0x8018CFDC)->mc_init_reset(0x80157368)->
cd_result_wait_a(0x801968CC)->cd_timer_read(0x8019A460), nesting via
per-block IRQ re-entry down to sp=0x801FF0F0 — inside the fixed 240B
record table 0x801FF080+slot*240. Stack spills write session pointers
(0x801BCB8C) onto rec+0x14; the coroutine's cd_intc_dispatch->
cd_cmd_queue_service->dev_cmd_round_dispatch->cd_cmd_state_commit then
jalr's the corrupted handler -> wild dispatch 0x801BCB94.

## Faithfulness analysis

- cd_file_load_sm/cd_result_wait_a/cd_timer_read are GENERATED (faithful
  MIPS->C); the nesting structure is real. cd_timer_read is delegated
  because its waits must yield to the CD coroutine (per-block IRQ checks).
- mc_init_reset is CLEAN-ported (game/save.c) -> runs on host stack, does
  NOT consume guest sp. So our guest sp is SHALLOWER than HW by that frame
  -> HW reaches DEEPER -> corrupts the record table AT LEAST as much.
- Conclusion: record-table overwrite during file-loads happens on HW too.
  Survival is an IRQ-dispatch-vs-corruption TIMING race, not stack depth.

## OPEN

Need oracle sp DURING an actual file-load (cd_file_load_sm breakpoint armed
on 4371; attract mode has not reached one yet). That decides whether HW's
file-load wait chain stays on main (deep, faithful) or moves to the
coroutine (would mean our synchronous path is the divergence).
