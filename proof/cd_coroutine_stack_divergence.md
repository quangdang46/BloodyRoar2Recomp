# CD deferred-coroutine stack divergence — THE crash root cause

## Oracle ground truth (DuckStation, port 4371)

Continuous `get_registers` sampling over ~6.29M samples / 4 min of gameplay
(includes CD command waits — `cd_result_wait_a` pc_break fired repeatedly):

- **min sp = 0x801BDC08** at `pc=0x8019A4A8` (inside `cd_timer_read`),
  `ra=0x80196B88`. That is the **0x801BDxxx deferred-coroutine stack**,
  reached via the CD IRQ -> `cd_ctx_restore` resume.
- Main-stack (`0x801FFxxx`) min sp = **0x801FFE80** — shallow only; the
  main thread NEVER nests deep during CD file-loads on HW.
- **rec_bad = 0**: over all samples the device-command record slot1
  (`0x801FF170`) `rec+0x14` NEVER held a session-data pointer — the record
  table is never corrupted on HW.

## Our runtime (bug)

`spdepth` ring shows the same `cd_file_load_sm -> mc_init_reset ->
cd_result_wait_a -> cd_cmd_issue -> cd_timer_read` chain nesting on the
**main thread (tcb=0x8FE0) at sp=0x801FFxxx down to 0x801FF0F0** — inside
the fixed 240-byte record table `0x801FF080+slot*240`. `recvalring` shows
`sw $sN` spills writing session pointers (0x801BCB8C) onto `rec+0x14`.
The IRQ-side `cd_intc_dispatch -> cd_cmd_queue_service ->
dev_cmd_round_dispatch -> cd_cmd_state_commit` then jalr's the corrupted
handler -> wild dispatch `0x801BCB94`.

## Conclusion

On HW the heavy CD wait/response work is **deferred onto the dedicated
coroutine stack 0x801BDxxx** precisely so the main stack stays shallow and
never touches the record table. Our runtime runs it **synchronously on the
main stack**, so it reaches 0x801FF0F0 and corrupts the records. The bug is
NOT stack depth, record placement, or command ACK timing — it is that the
**coroutine stack-switch (cd_ctx_save/cd_ctx_restore -> 0x801BDxxx) is not
engaging** for this CD path. Faithful fix: make the CD IRQ/deferred-work
resume actually switch guest sp onto the coroutine stack like HW.
