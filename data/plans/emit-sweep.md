# Plan: parallel port of render/GPU generated functions (0x80186000-0x80190000)

Goal: drive generated/ coverage to 0 by clean-porting the 89 unported
render/GPU functions, split across 10 Devin peers (one worktree+branch each).

## Scope
- ONLY guest addrs 0x80186000-0x8018FFFF (render/GPU emit family).
- Do NOT touch 0x8019xxxx+ (CD/IRQ/kernel = §4 delegation territory).
- Per-function: if body needs IRQ-timing/deferred-coroutine/BIOS-kernel
  semantics clean C cannot express -> br2_call_generated delegation (§4).
  Leaf GTE/OT emitters -> faithful clean C (proven pattern in game/gfx.c).

## File ownership (collision-free)
- Each peer writes ONLY its own NEW file: game/gfx_emit_pNN.c (NN=00..09).
- Shared game/renames.map: each peer appends its <addr> <name> lines.
- game/fnmap.c + src/clean_table.c: each peer regenerates locally to build;
  root regenerates the union at merge.

## Per-peer deliverable
1. game/gfx_emit_pNN.c — self-contained (preamble + local static helpers
   swl_guest/prim_ot_link_indexed/DB32/DB16 + ported fns, non-static).
2. game/renames.map += sorted entries.
3. python3 game/gen_fnmap.py && python3 game/gen_clean_table.py
4. cmake build clean.
5. Headless runtime verify (frame advance, no crash).
6. Commit to branch orch/emit/pNN.

## Merge (root)
- Merge all orch/emit/pNN branches; union renames.map; regen fnmap/clean_table;
  build; runtime verify versus+attract.
