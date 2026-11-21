# GPU GP0 stray word 0xFE1F220E — investigation (agent 3647246a)

## Verdict
`0xFE1F220E` is **guest RAM content faithfully transported by DMA** — NOT a
runtime DMA/GP0 decode artifact. The corruption is upstream: a stale/bad
word sits in a GPU packet payload at a position the GP0 parser treats as a
command boundary.

## Evidence
- Prior raw dump (FAITHFUL_TIMING_PLAN SESSION9f): word sits INLINE between
  complete, correctly-sized commands — a valid 4-word VRAM->VRAM cmd ends
  exactly before it, valid prims resume after. A runtime word-count/address
  error would desync the whole tail, not emit one stray word.
- Runtime walker `dma_gpu_ll.c:95-160` verified faithful (tag top-byte len,
  payload at +4+i*4, next=low24, term 0xFFFFFF) vs psx.wiki.
- GP0 decoder `gpu.c:4747-4833` fixed lengths correct; `>=0xE7` NOP fix
  (ff4f78f) is why it no longer halts — stray is silently NOP'd.
- Did NOT recur in ~190k frames of title/menus/attract/FMVs — trigger is
  combat/battle-specific content.

## Combat blocker found (separate CD bug)
Versus nav -> gs=8 ph=6 (char select); arcade -> gs=8 ph=3; both stall with
`cdrom_state`: sector_read_pos=2060/2340, sector_available=1 UNCONSUMED —
the "CD consumer task starved" wedge (commit a015713). Prevented live
capture of the corrupting frame.

## Static audit — all OT emitters consistent
spr_attr_alloc_a/b, sprite_node_emit_a/b, sprite_obj_alloc_emit,
sprite_quad_emit, gpu_prim_pkt_build, prim_packet_init_tex,
gpu_pkt_tag_60/74, sprite_rect_set_a/b/c, ot_packet_chain_fixup — every
tag-length byte matches payload word count. No emitter bug found.

## Leading hypothesis (needs live corrupt frame)
Packet pools recycled WITHOUT clearing: spr_pool_a_alloc (gfx.c:1425),
spr_pool_b_alloc (gfx.c:1452) return slot+4 raw behind a frame-sentinel.
If a builder leaves a payload word unwritten (conditional path / off-by-one
in multi-tag chain, e.g. puVar1[6] sub-tag gfx.c:1234), GP0 gets a stale
word. 0xFE1F220E decodes as plausible vertex s16 pair (0x220E=8718,
0xFE1F=-481) — consistent with STALE COORDINATE data.

## Next step
Once CD wedge fixed + combat reachable: gpu_frame_dump the GP0 ring entry
for the stray cmd -> read src_addr/pc/func/ra -> wtrace_arm/gwatch that
address -> store PC identifies the exact guest fn writing the bad word.

## UNIFIED THEORY (parent)
The combat CD stall (sector unconsumed) is the SAME consumer-starvation as
the FMV wedge. Failed/incomplete resource loads leave packet-pool slots
uninitialized -> stale coordinate word -> 0xFE1F220E. Fixing the CD
consumer likely fixes BOTH the FMV stall AND the GPU corruption.

## POST-CD-FIX STATUS (session update)

The CD consumer-starvation wedge was root-caused and fixed in `be17669`
(`cdrom_cycles_to_irq` under-advertised the armed i_stat-bit2 presentation
deadline → fast chunker stepped over CD IRQ boundaries → deferred
command-issue missed its doorbell → stream/file-load producer starved).

**Unified theory now testable:** incomplete resource loads were leaving
`sprite_pool_a/b` packet slots uninitialized → stale coordinate word →
stray GP0 command. With CD fixed, loads complete → pools initialize.

**Evidence gathered on live versus combat (`:8125`, gs=8, ~340fps):**
- Fight runs cleanly at full speed — NO CD stall (the trigger condition
  for incomplete loads is gone).
- `gpu_opcodes` cumulative histogram across the ENTIRE run (incl. all
  versus combat): only standard GP0 opcodes 0x00-0xE6; **zero entries in
  the 0xE7-0xFF NOP/reserved range** where stray `0xFE1F220E`-class words
  would appear. No anomalous opcode distribution.
- `gpu_frame_dump` sample combat frame: 0 anomalous entries.

**Verdict: LIKELY RESOLVED as CD-wedge fallout.** Not 100% conclusive —
the stray word is intermittent, value-varying (stale pool data), and
combat-specific, so absence in one histogram window isn't proof of absence
forever. If it recurs: re-run the documented next step (gpu_frame_dump the
stray frame → src_addr/pc/func/ra → wtrace_arm/gwatch the writer). The
packet-pool recycling hypothesis (spr_pool_a/b_alloc uninitialized slots)
remains the leading mechanism if a stray reappears.
