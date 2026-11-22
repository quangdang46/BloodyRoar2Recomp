# PSX hardware semantics: Pause(0x09) does NOT instantly kill sector delivery

Source: psx-spx CDROM reverse-engineering test cases
(https://problemkaputt.de/psxspx-cdrom-response-data-queueing.htm) +
psx.wiki CD-ROM command docs. Cross-referenced against
`psxrecomp/runtime/src/cdrom.c` `exec_command` case `0x09`.

## Documented hardware behavior

Test sequence (sector buffer vs Pause):

```
Setloc(0:2:0)+Read
Process INT1 --> receives sector header for 0:2:0
Delay(1)
Pause
Process INT1 --> receives sector header for 0:2:1 (oldest)
Process INT3 --> receives stat=22h (first pause response)
Process INT2 --> receives stat=02h (second pause response)
```

Key documented facts:

1. **"despite of Pause, the CDROM is still writing to the internal buffer"**
   — sectors already read into the 32Kx8 sector buffer continue to be
   written/overwritten after Pause is issued. In one variant the buffer
   was overwritten by sector 9 before Pause's INT3 was even processed:
   *"this might be because the Pause command isn't processed at all until
   INT1 is processed."*
2. Response interrupts are **queued**, not OR-ed: INT1 (data-ready) for the
   next buffered sector can be delivered BEFORE Pause's INT3 ack, and INT2
   (complete) only after INT3 is acknowledged.
3. Pause's first response shows the read bit still set (stat=0x22), the
   second shows it cleared (stat=0x02).
4. ReadN/ReadS "raise one INT1 per sector until CdlPause stops them" —
   Pause stops the *stream*, but the in-flight/pipelined sectors still
   complete their data-ready notifications.

## Divergence in our runtime

`exec_command` case `0x09` (`cdrom.c:2301-2314`) executes
`stop_read_stream()` + `cdrom_clear_pending_dataready()` **synchronously at
command dispatch**:

- `reading=0`, `read_delay=0`, `s_cd_timing_next_due=0` — the sector
  pipeline halts instantly; any sector mid-read never completes.
- `pending_dataready=0` — a data-ready INT1 already pended (sector
  delivered, IRQ not yet presented) is **destroyed**. On hardware that INT1
  would still be delivered (queued behind/ahead of Pause's responses).
- `stat_reg &= ~(CDSTAT_READ|CDSTAT_PLAY)` applied immediately, so even the
  ACK response may report a state hardware wouldn't show yet.

If BR2's driver counts sectors via INT1/data-ready (its channel-entry
finalize gate at `0x80199440` requires the expected sector sequence), a
Pause issued while a sector was in flight or its INT1 pended drops exactly
one notification → the channel entry never reaches the finalized seq tag →
producer=2/finalize=0 forever → no ch4 file-data DMA → no
`DeliverEvent(0xF0000009)` → `TestEvent(0xF1000008)` spin. Matches
`proof/cd_event_drop_point.md` precisely.

## Faithful-fix direction (for runtime agent to confirm vs Beetle oracle)

Pause should be a **pipeline-drain**, not an instant kill:

- Mark pause-pending; let the current in-flight sector finish and its
  INT1 deliver (queued per response-interrupt rules).
- Preserve `pending_dataready` (do not clear it in stop path for Pause —
  hardware keeps the buffered-sector notification).
- Only after the pipeline reaches the pause point: clear READ stat, arm
  INT2 completion after `pause_complete_delay_cycles()`.
- Ordering must respect the response queue: a pending INT1 may precede
  Pause's INT3.

Whether BR2's wedge is exactly this depends on whether a sector/INT1 was
in flight at Pause time — the runtime agent's descriptor dump + Beetle
oracle comparison on port 4999 is verifying.
