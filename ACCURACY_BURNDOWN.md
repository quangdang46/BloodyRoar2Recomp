
## Session 16h — CD stream +4 credit: full mechanism decoded via annotated disasm

Read `psxrecomp/docs/internal/cdmc_80199440.asm` (human-annotated MIPS). Decoded
the complete dir-entry streaming state machine `cd_mc_state_machine` (0x80199440):

**Dispatch (top):**
- `fc69c==1` -> early return (completion pending)
- `fc688!=0 && flag` -> re-arm `fc68c`, return 1
- else `jal 0x80195d44` status; entry = `fc6c0 + fc6a4*0x20` cached to `FEB18`
- `entry->state(+0)==0` -> CLAIM path (0x80199574); `state!=0` -> return 4 "busy"

**Claim path (state==0):** writes entry tag to `+0x1c`, validates, then issues the
first read. `fc6b0==0` -> `cd_chan_cmd_submit(ch3, madr=entry, bcr=8, chcr=t0)`;
`fc6b0!=0` -> `cd_copy_u32`. Then GPU-packet stores + state bookkeeping.

**In-progress path (+4>0, 0x80199b10):** computes double-buffer slot
`fc6bc = fc6c0 + stride*0x20 + cursor*2016` (=0xBE0 for ent0, MATCHES live ch3
madr). Reads `entry+6`(expected) & `entry+4`(progress). If `progress==expected-1`
-> sets `fc69c=1`, issues final read, `fc6a0++`, tail sets `state=3`, `cursor++`,
`jal 0x801990f0` (cd_dir_entry_finalize) when `fc69c`. Else issues `0x1f8` read.

**+4==0 path (0x801998c4):** reads `entry+8`(field8/LBA), compares `fc6b4`;
either skip (`jal 0x80199320`, state->0, callback) or bounds-check consume
(`(fc6c4-fc6a4-1) < entry+6`) -> sets state=1 + callback, OR falls to fc6a8=fc6a4.

**CONFIRMED: `entry+4` is READ but NEVER WRITTEN inside cd_mc_state_machine.**
All writes are to `entry+0` (state). `+4` is credited EXTERNALLY by the
read-completion path.

**Live DMA ground truth (`dma_cdrom_history`, 469 entries):**
- ALL 256 captured transfers are `requested_words=512` (2048-byte bulk sector
  loads), `completed=1`, sequential LBA/addr, frames ~306-436. A large file load.
- ZERO `0x1f8`-byte or `bcr=8` stream transfers EVER reached the DMA engine.
- Live ch3: `madr=0xBE0 bcr=0x1F8 chcr=0` — programmed but never transferred.

**Runtime DMA path verified faithful:** `cd_chan_cmd_submit` writes MADR/BCR,
spins on `*(0x1F801800)&0x40` (=data_fifo_ready = request&BFRD && rb_available),
writes CHCR. `try_execute` needs CHCR bit24 + `channel_enabled(3)`(DPCR bit15).
`execute_ch3_cdrom`->`start_async_cdrom_transfer`. `rb_available()` true live
(pos=0<size=2340), request=0x80 BFRD set -> data_fifo_ready TRUE -> wait passes.

**Narrowed root cause:** the stream-read ch3 submits either (a) never reach
`cd_chan_cmd_submit` because ent0 is already `state=3` (busy -> return 4) and the
claim path only fires on `state==0`, or (b) the submit's FIFO spin or the DMA
kick fails silently. ent0 `state=3,+4=0` means it was claimed+first-read-issued
earlier but `+4` never credited -> the FIRST read's completion is the break.

**Next:** determine whether ent0's first `cd_chan_cmd_submit(ch3,bcr=8)` DMA
actually kicked (check DICR/DPCR bit15 + cdrom_async.active at submit time), and
find the +4-crediting read-done handler (likely the ch3-DMA-done IRQ path or a
paired consumer of the 0xBE0 double-buffer).

## Session 16i — PRIME SUSPECT: stale $t0 -> wrong CHCR in fc6b0==0 claim path

In `cd_mc_state_machine` claim path (state==0 entry), the `fc6b0==0` branch
(0x80199674) calls `cd_chan_cmd_submit(ch3, madr=entry, bcr=8)` and passes CHCR
via `sw $t0,0x10($sp)` at 0x80199688. **$t0 is never initialized on this path** —
`lui $t0,0x1100` only executes in the sibling `fc6b0!=0` branch (0x80199644) and
the `+4>0` path (0x80199b70/0x80199ba4).

Chicken-and-egg hypothesis:
- First stream read (claim path) needs CHCR bit24 set to kick ch3 DMA.
- Valid CHCR (0x11000000) is only produced by the `+4>0` path's `lui $t0`.
- `+4>0` requires a completed read; the first read's DMA never kicks if CHCR is
  stale -> `+4` stays 0 -> `+4>0` never reached -> `t0` never set -> livelock.

On hardware t0 is caller-saved and plausibly already 0x11000000 from prior code;
in the recomp the value depends on coroutine/fiber register preservation across
the state machine's continuation re-entry. If the fiber scheduler does not
preserve $t0 (or the claim path runs before any +4>0 block in the same
activation), CHCR is garbage -> try_execute sees bit24 clear -> no DMA -> +4=0.

VERIFY: dump $t0 at 0x80199688 on a live run (gpr trace / watchpoint), and check
whether cd_chan_cmd_submit's CHCR arg has bit24. If stale, the faithful fix is to
confirm the original binary's t0 provenance (it may be set by jal 0x80195d44's
callee or an earlier block in the same activation) — NOT to hardcode CHCR.

=== RESOLVED: dev_byte_xfer SIO device-ptr mistranslation (commit 99b190d) ===
ROOT CAUSE: clean dev_byte_xfer (0x801a1f74) read the SIO device struct/ctrl
pointers from 0x801be3e4/0x801be3e0 — but the real globals are 0x801be7e4/
0x801be7e0 (-6172/-6176 off the 0x801C0000 base; the port had a 0x400-low
typo, same class as the cd_channel_table_init 0x6000-base bug). be3e0/e4 sit
in an unrelated .data table (arithmetic sequence 0x840c83fd..0x85d985c9), so
*(0x801be3e4)=0x85208510 -> mem_ptr wild deref -> segfault during CD/SIO init.
The real pointers hold MMIO addrs baked into .data: be7e4=0x1F801040 (SIO_DATA
dev struct: [0]=byte buf,+4=status u16,+0xe=cmd u16,+0xa=u16) and
be7e0=0x1F801070 (I_STAT ctrl u32 bit7/bit2). Body also busy-polls hardware
FIFO status in while-loops needing IRQ/DMA-driven updates.
FIX: delegated whole body to generated br2_call_generated(0x801a1f74) —
mem_ptr() routes 0x1F801xxx to the open-bus mirror (guest_at has no MMIO
backing) and a clean-C spin never yields to psx_check_interrupts_at ->
livelock; same contract as cd_timer_read/cd_mc_state_machine/cd_channel_init.
a0 is a host ptr into guest RAM; guest_of() recovers the guest $a0.
VERIFIED: game now reaches frame 1718 (was crashing in dev_byte_xfer ~frame<100).

=== NEW BLOCKER @frame1718: wild dispatch to low-KUSEG 0x27AC ===
symptom: FAIL-FAST unknown dispatch addr=0x27AC ra=0x8019A53C (vsync_wait_n
caller cont), last_func=0x29CC, epc=0x8019A600, frame=1718. Single miss
(unknown_dispatch_log count=1), k0(gpr26)=0x27AC, sp=0x801FFF78, a0=0x60C
a1=0x8000. Region 0x2000-0x2800 is the PSX kernel/low-RAM segment (kernel
fns like OpenEvent/TestEvent land ~0x2500-0x3000); 0x29a0-0x29fc is clean
MIPS (lw struct fields, jr ra @0x29c4, fn entry @0x29cc). 0x27AC itself
holds 0xd8000000 inside a data-table-looking run (repeating 0x??800507 /
0x??e00008) -> likely jumped to a DATA word = bad kernel/interrupt pointer,
OR a recompiler discovery gap for kernel code the static pass never found.
NEXT: identify the jumping insn (jr/jalr through $k0 or a kernel fn-ptr
table) — likely another uninit callback/trampoline in the SIO/memcard or
kernel-call path; check whether 0x27AC is code needing a seed vs corrupt
state. PSX_FAIL_FAST_UNKNOWN_DISPATCH=0 survives-and-logs for enumeration.

=== DIAGNOSED: frame-1718 wild dispatch 0x27AC (kernel-RAM overwrite) ===
MECHANISM (verified): 0x27AC is a REAL dispatch-table key
(OpenBIOS_func_000027AC, kbless kernel window RAM[0x500,0x4BA0) <- ROM
0x1E4D4). The kernel's exception/interrupt RETURN is
OpenBIOS_func_000029CC: it restores the full GPR set from a saved context
block via $k1, loads $k0=*(k1+128) (the saved resume-EPC slot), then
`jr $k0; rfe` -> cpu->pc=$k0. The saved resume PC was 0x27AC.
ROOT CAUSE: live kernel RAM at 0x2700-0x27c0 & 0x281c-0x285c does NOT match
the ROM image (bless->MISMATCH). Region holds a runtime-written DATA
structure (repeating 0x??800507/0x??e00008 pattern; NOT present anywhere in
openbios.bin -> not relocated code). 0x27c4-0x2818 still matches ROM.
So a kernel data object (TCB/event/install table) overwrote relocated kernel
CODE. On return, jr $k0 -> 0x27AC reads 0xd8000000 (op 0x36, unassigned
R3000A) -> dirty-RAM interp can't decode first insn (insns==0) ->
dirty_ram_dispatch returns 0 -> psx_unknown_dispatch FAIL-FAST.
Page 0x2000 IS dirty (bitmap word0=0xffff2fff) so interp WAS reached; the
failure is purely the non-decodable first word = data where code was expected.
NEXT: catch the writer of 0x2700-0x27c0 (gwatch armed) — likely a kernel
object alloc with wrong base/size during CD/SIO/memcard activity. Alternatively
the saved-context $k1 block itself may be corrupt (offset128 read wrong).

=== WRITER IDENTIFIED: heap_serialize writes kernel RAM (frame 1717) ===
PSX_GWATCH on 0x2790+208 caught it: at frame 1717, store_pc=0x8016A8D4
(`sw $v0,0($a1)` = 0xACA20000, plus swl/swr unaligned copies) inside generated
br2eu_func_8016A82C wrote the 0x??800507 data pattern over relocated kernel
code at 0x2797-0x27BF. Caller: game fn @0x8016C594 does `jal 0x8016A82C`
with $a1=$s4 (serialize-object ptr), $a0=$s1, $a2/a3=*(s2+8/12).
0x8016A82C = heap_serialize_tag_01000000 (clean menu.c wrapper ->
br2_call_generated; NOT in clean_table -> faithful generated body runs).
So the wild write is FAITHFUL guest behaviour: $s4 pointed into kernel RAM
[0x500,0x4BA0). On real HW the game works, so $s4 SHOULD point to a normal
buffer -> the pointer is wrong upstream (caller 0x8016C5xx sets $s4, or the
serialize object's dst field is mistranslated/corrupt).
ALTERNATIVE: game legitimately uses a kernel-allocated buffer (alloc_kernel_
memory / kernel-call) and the collision is a kernel-layout difference — but
then real HW would corrupt the same code, so unlikely.
NEXT: trace $s4 (gpr20) provenance in caller fn @~0x8016C5xx; check whether
serialize dst comes from a clean-port field read (guest_at missing?) or a
kernel-alloc return. The 0x??800507 pattern = a table of ptr/offset entries.

=== REFINED: tag_01000000 write-cursor fully decoded + traced ===
heap_serialize_tag_01000000 (gen 0x8016A82C) has TWO copy phases. The
corrupting store is phase-2 `sw $v0,0($a1)` @0x8016A8D4. Phase-2 write cursor:
  a1(gpr5) = MB32(rp+0x18)*8 + *(sp+0x14)   [= idx(+0x18)*8 + arg6]
  arg6 = MB32(ctx+0x14)  (stack arg, staged fr+0x14 == sp+0x14; verified
  br2_guest_frame math is correct). arg1 = rp+4, so idx=*(rp+0x18),
cnt=*(rp+0x14). a0=cur is the separate output-stream cursor (0x8005/6xxxx).
Added decomp_trace tags 27/28 (hser_1000_ctx/hser_1000_rec) in clean
heap_serialize's 0x01000000 case capturing ctx, MB32(ctx+0x14), MB32(rp+0x18)
AT CALL TIME. RESULT: all 30 calls produce a1 in 0x80001D1D..0x80002727 —
VALID 0x8000xxxx table addrs (ctx=0x800F95D8 base 0x80001D1D; ctx=0x800F061C
base 0x80001D6F). NONE reach 0x2790.
=> The corrupting tag_01000000 call does NOT go through clean heap_serialize.
It runs via GENERATED heap_serialize (br2eu_func_8016B7C0) reached through a
dispatch path that skips clean_try_dispatch (heap_record_emit 0x8016CC5C is
NOT in clean_table — only fnmap; psx_dispatch_call/CPS-continuation may bypass
clean). Both clean+generated stage identical faithful args, so the divergence
is WHICH blk/records each processes or corrupt heap-record input data.
NEXT: (a) confirm generated heap_serialize actually runs (add a generated-side
marker or count hser vs total heap_serialize calls); (b) if so, the record
rp+0x18/ctx+0x14 for the bad blk is corrupt upstream (decompress/heap_alloc54/
heap_ptr_fixup) — trace the heap-record BUILD for that blk; (c) alternatively
a mid-function CPS re-entry at block_8016A8A8 with clobbered gpr5.

=== SESSION: crash-chain cleared -> stable 11k-frame sim, zero render prims ===
Fixed + pushed (all verified against guest MIPS / faithful semantics):
- 0cd160f GTE/COP2 wired to real gte.cpp (setCopReg/getCopReg/copFunction ->
  gte_read/write_data|ctrl + gte_execute); isqrt_refine UB->brk trap fixed by
  faithful MIPS port (digit-recurrence sqrt, i==4 special step).
- 8e6042c task_slot_fns_run slot-fn arity: guest jalr passes a0=&ctx cell,
  a1=node_head; clean called fn(a0) only -> a1 garbage (0x1f deref).
- 8b49349 node->f8/f10 generic dispatch -> task_invoke (correct int-vs-ptr
  arg convention; was raw-guest-addr passed to host-ptr fns -> 0xffffffff..).
- 125ab16 frame_divisor_get: cached g_DAT_1f800019 pointer miscompiled inside
  huge battle_intro_flow state machine (x28 clobbered) -> direct
  *(uint8_t*)guest_at(0x1f800019) = faithful scratchpad read.
- 0e91caa fx_record_alloc: iVar2 (guest addr 0x801d0508) cast raw to host ptr
  -> sign-extended 0xffffffff801d0508; now guest_at(iVar2). NOTE: the
  (uintptr_t)int_holding_KSEG0_addr -> host-ptr bug class is SYSTEMIC (~86
  sites); only bites when value is a guest addr, most sites are correct.

RESULT: game now runs 11000+ frames @50Hz STABLE (was crashing in battle-intro).
CD streaming live (seq 447K, normal seek/read/status track1 ~LBA3300, finished
~frame7929). Task scheduler dispatching real fns (radial_glow_draw etc).
GPU: gp0_draw=10422, gp0_env=58823 counted BUT gte_verts=0, ovh_prims=0,
ot_rank=-1, VRAM all-zero, screenshot all-black.

NEW FRONTIER (next target): game simulates correctly but submits ZERO draw
primitives / empty OT to GPU. Either (a) game legitimately in black sub-state
waiting for memcard/controller/input, or (b) OT/primitive-list build broken
upstream (a clean-port guest_at miss leaving OT empty), or (c) GPU OT-scan
not finding the ordering table the game wrote. Diagnose: check whether game
issues GPU "draw OT" cmds (0x02/0x03) and whether OT base in RAM is populated.

=== SESSION2: stable sim -> battle state reached, OT empty (black) ===
Post-crash-chain the game no longer crashes: 18000+ frames @50Hz stable.
CD/memcard/SIO/tasks/GPU-cmd-path all confirmed live via debug server.
Rendered real content frames ~500-1500 (boot logos, prims 0x2C/0x64+texpage),
then went permanently black ~frame1999. Every frame since: identical 7 GP0
cmds = draw-area setup + ONE black fill (0x02), ZERO polygon prims.
g_game_state (DAT_1f800000) = 5 -> battle_intro_flow per-frame loop
(sample: battle_intro_flow menu.c:1969 -> frame_tick game.c:1335 ->
guest 0x8019A5D8). frame_tick calls gpu_draw_otag(iVar9+0x207c) EVERY frame
= submits the ordering table. gpu_draw_otag -> gpu_call(8,GPU_TBL()[6],ot)
-> gpu_invoke(t[2], t[6], ot_addr) (gfx.c:101-107,419). Same gpu_call path
emits the env cmds that DO reach GPU, so submission works.

DIAGNOSIS: the ordering table is EMPTY -> scene builder emits no packets.
The battle 3D scene (arena + fighters) never populates the OT. Not a GPU or
present bug (VRAM genuinely zero, not a readback artifact). The game reached
battle state (via attract/scene_entry) but the render-list build produces
nothing -> fighter/arena/animation render subsystem is the missing piece.
NEXT: trace the scene/OT builder (what walks fighters+arena emitting prim
packets into OT) — likely a clean-port task/anim/render path not running or
its data ptrs broken. Check whether scene_entry loaded battle assets and
whether the per-frame prim-emitter task is in the task pools.

=== SESSION3: black-screen root -> empty OT; dev_byte_xfer serial spin ===
Deep trace of the black screen:
- Game reaches battle state; battle_intro_flow->battle_frame_tick(0x1ff)
  runs prim_list_gte_invoke/persp_step. Fighter slots populated
  (0x03001101/0x02011201), prim lists populated (model ptrs 0x8004xxxx,
  packet chains 0x801E1xxx). Scene DATA exists.
- BUT gpu_draw_otag submits OT @0x801e82dc which has ~1 entry -> packet
  @0x801ec588 that is NOT GPU primitive data (linked struct records).
  Every frame = 7 cmds (draw-area + 1 black fill), ZERO poly prims.
  => OT never populated with drawable packets = scene builder emits none.
- clean_trace: 97.8M calls, ~100% = fifo_space_check(0x801A4230) from
  dev_byte_xfer(0x801A1F74, ra 0x801A1FFC). dev_byte_xfer = SIO/memcard
  byte send/recv. tx_writes=1.5M climbing ~500/s, sio_rx occasionally 0x41.
  fifo_space_check reads TIMER2 (0x1f801120/124/128 = T2 count/mode/target)
  as a timed drain-wait; base(0x801ff260) is rewritten constantly so
  elapsed never accumulates -> returns 0 (keep polling) ~forever.
  This is a SIO/memcard byte-transfer throttle spinning. Likely a BACKGROUND
  card op (frames still advance) OR a genuinely slow/stuck card transaction.
  mc: 114 cmds, 108 reads done, both slots MC-magic present, dirty:false.

TWO SEPARATE ISSUES:
(A) dev_byte_xfer/fifo_space_check spin — memcard/SIO transfer not
    completing at reasonable rate. Investigate: is it a legit huge/slow
    card op, or does the card fail to ACK/drain so the game retries?
    Timer2-based wait may be mis-computed (base reset semantics).
(B) PRIMARY visible blocker — battle OT stays empty, no prims submitted.
    Scene builder (fighter/arena -> OT packets) produces nothing despite
    populated prim lists. Next: find the per-frame prim emitter and why
    it emits zero (task not dispatched? transform/GTE producing offscreen?
    OT link broken?).

=== SESSION3b: black-screen = battle_intro_flow stuck on bad sub-state ===
Root of black screen narrowed further:
- battle_intro_flow is a phase SM keyed on DAT_1f800001 (shared u8 sub-state
  byte, scratchpad+1). Valid intro phases: 0=setup(fighter select+spawn),
  1=menu_res_load_sm, 2=countdown->battle_frame_tick(0x1ff/0x1fb)->0xf0,
  0xf0/0xf1=shutdown->frame_divisor_get->0xff->exit.
- Live DAT_1f800001 = 0x6c (108) — NOT a valid intro phase. Falls through
  every branch to LAB_801677b4 = frame_tick+round_banner_draw = BLACK.
- DAT_1f800001 is written by MANY state machines (menu.c sets it to
  0..0xf,0xf0,0xf1,0xff,-1 across title/transition/select flows). On entry
  battle_intro_flow expects it ==0 (fresh) to run setup; 0x6c means a
  PRECEDING state left a non-intro value -> intro never initializes the
  battle scene -> empty OT -> black.
- Scene data IS present (fighters spawned 0x03001101/0x02011201, prim
  lists 0x801BEFF0 populated with model ptrs 0x8004xxxx + packet chains).
- SIDE ISSUE: dev_byte_xfer(0x801A1F74)=SIO/memcard byte xfer spins in
  fifo_space_check(Timer2 timed drain-wait); base(0x801ff260) rewritten
  constantly -> never accumulates -> ~98M polls. tx_writes=1.5M ~500/s.
  Likely background card op OR stuck card transaction; frames still advance.

NEXT (highest value): find which preceding state should reset
DAT_1f800001=0 before battle_intro_flow (scene_entry/state4->5->6 path),
or whether battle_intro_flow(param_1==0) must self-init the phase. Compare
against guest MIPS: does the real game zero DAT_1f800001 on battle entry,
or does a specific predecessor own it? Then the OT/scene build unblocks.

=== SESSION3c: DAT_1f800000/001 overlap suspicion ===
Declarations (bind.c:1032-33, game.c:210-212, menu.c:340):
  g_DAT_1f800000 -> guest 0x1f800000 ; DAT_1f800000 = *(u32*) = g_game_state
  g_DAT_1f800001 -> guest 0x1f800001 ; DAT_1f800001 = *(u8*)  = sub-state
They are ADJACENT: DAT_1f800001 is byte1 of the DAT_1f800000 u32.
Live scratchpad bytes 05 6c 00 00 -> u32 g_game_state = 0x00006c05 (NOT 5),
byte1 DAT_1f800001 = 0x6c.
=> If g_game_state is faithfully a u32, byte1 pollution (0x6c) corrupts it
to 0x6c05 -> switch(uVar4<0xe) fails -> wrong/absent case. If g_game_state
is meant to be a u8/byte0-only read, the *(u32*) cast in game.c:210 is a
PORT BUG reading 4 bytes and mixing the sub-state byte into game_state.
This is now the PRIME suspect for the black screen: either (a) the u32
read over-reads into DAT_1f800001, or (b) DAT_1f800001=0x6c itself is a
stale/garbage sub-state the intro SM can't handle.
NEXT: disasm guest @0x80126370 region (g_game_state switch) to learn the
REAL access width (lbu/lh/lw) for g_game_state and DAT_1f800001, then fix
the port's DAT_1f800000/DAT_1f800001 access widths to match exactly.

=== SESSION3d: RESOLVED access width + real blocker isolated ===
CONFIRMED via generated MIPS (br2eu_func_80126234 / _80167330):
  g_game_state @0x1f800000 is a UINT8 — guest uses write_byte/load_byte
  (sb/lbu), NOT lw/sw. DAT_1f800001 @+1 is a separate u8. FIXED in
  game/game.c: DAT_1f800000 u32->u8 (commit ebe4bb6). The u32 read
  produced 0x6c05 and every `g_game_state=N` u32-store clobbered bytes
  1-3 (incl DAT_1f800001). Faithful width now matches guest.
POST-FIX live state: g_game_state=5, DAT_1f800001 cycles (was stable
0x6c; forcing it to 0 via write_ram lets the phase SM run 01->F1->B9).
DIAGNOSIS: DAT_1f800001=0x6c has no battle_intro_flow case -> falls to
LAB_801677b4 frame_tick+round_banner = black clear. BUT forcing it to 0
only moves the stuck value (0x6c->0xB9); gp0_draw stays FROZEN at 10422
since boot logos. => DAT_1f800001 is a SYMPTOM, not the cause.
ROOT BLOCKER: the battle/scene render pipeline emits ZERO primitives.
Ordering table never populates despite fighters spawned + prim lists
populated. battle_frame_tick->prim_list_gte_invoke/persp_step either
never runs (phase!=2) or runs but emits nothing. Remaining work = the
fighter/scene/.ANI/.ACT/event-VM/render RE backlog.
SIDE: dev_byte_xfer SIO spin (fifo_space_check Timer2 timed-wait,
base@0x801ff260 rewritten per-poll) ~18M calls dominates clean_trace;
frames still advance @50Hz. Secondary.

=== SESSION: +0x13 render-gate isolated; root = SIO/memcard read wedge ===
RENDER GATE FOUND: scratchpad byte 0x1f800013 (+0x13 off the 0x1f800000
battle object) is the draw-enable flag. BOTH frame_tick (game.c:1746-area)
and frame_present (game.c:1746) gate `gpu_draw_otag(ot)` on
`*(char*)(param_1+0x13)!=0`. It is maintained ONLY by:
  - frame_advance_helper -> scratch_013_set(1)  (game.c:1097)
  - load/decode wrappers: save uVar1=+0x13; scratch_013_set(0); ...work...
    scratch_013_set(uVar1)  (scene_entry:1586, menu.c, save.c ~20 sites)
NEVER set inside frame_tick/frame_present/battle_intro_flow.
battle_intro_flow runs an internal do-while calling frame_tick forever but
NEVER calls frame_advance_helper -> +0x13 frozen at whatever it was on entry.
LIVE: +0x13=0 persistently -> gpu_draw_otag skipped -> OT built but never
submitted -> black. Forcing +0x13=1 via write_ram: gp0_draw 10422->13932
(+3510 draw cmds reach GPU) => OT submission path is CORRECT.
BUT screenshot still black (0.1% nonzero px) => prims submit yet rasterize
nothing visible => the OT/scene DATA is incomplete: the resource load that
populates it never finished.
ROOT CAUSE (upstream): SIO/memcard byte-transfer WEDGE.
  clean_trace = 99.6% fifo_space_check (0x801A4230); tx_writes climbing
  ~500/sec (238k->239k); mc_read_done FROZEN at 108; sio_rx=0xFF.
  dev_byte_xfer (0x801A1F74) spins in a Timer2 timed-wait per byte; the
  memcard read protocol (sio.c MC_CMD 0x52 -> ID1/ID2 -> ADDR -> READ_DATA
  128B -> CHK -> END) desyncs: card ACKs each byte but read never reaches
  MC_READ_END. Game keeps retransmitting -> load never completes ->
  +0x13 stays 0 (suspended block) AND scene/fighter geometry never loads.
fifo_space_check (0x801A4230): reads T2 counter 0x1f801120, base@0x801FF260,
lim@0x801FF264(=0x3C=60); elapsed=(T2count16-base)>>3 >= lim. base is the
last-activity timestamp refreshed per sent byte -> during a continuous
stalled stream it never lets elapsed reach lim -> returns 0 -> spin.
NEXT: fix the memcard read-protocol desync (why card RX=0xFF / protocol
stall) OR the dev_byte_xfer FIFO-wait so the pending load completes and
+0x13 restores. This unblocks BOTH the flag AND the scene data.

=== dev_byte_xfer / fifo_space_check internals (clean+gen decoded) ===
dev_byte_xfer (0x801a1f74) is GENERATED-delegated (save.c:9124 ->
br2_call_generated) because clean-C can't do the MMIO busy-poll faithfully.
fifo_space_check (0x801a4230) IS clean-ported (save.c:9138):
  a0=u16@0x1f801120 (T2 counter, LIVE sysclk timer - verified advancing,
  mode toggles 0x0400<->0x1C00); base=u32@0x801FF260; lim=u32@0x801FF264(=0x3C);
  if a0<base: a0+=u16@0x1f801128(or 0x10000); v0=a0-base; if(mode@0x1f801124
  &0x200)==0 v0>>=3; return v0>=lim.
dev_byte_xfer writes base=gpr[4]@0x801A2078 and lim=gpr[2]@0x801A2070 ONCE per
wait-setup (0x80200000-3488/-3484) - NOT per-poll. So the wait is correctly
armed; it should complete after ~lim ticks.
LIVE: tx_writes climbs ~500/sec steady, mc_reads=111 mc_read_done=FROZEN 108,
sio_rx=0xFF. +0x13 forced to 1 PERSISTS (nothing re-clears it) and gp0_draw
rises, but screenshot stays ~0.1% non-black => OT submits draw cmds yet
rasterizes nothing => scene/fighter geometry data is ABSENT (load never
populated it), NOT a rasterizer bug.
OPEN: is the SIO transfer a genuine wedge (protocol desync -> infinite
retransmit) or a legitimately huge/slow stream? mc_read_done frozen says the
pending op is NOT completing a card read. Next: identify WHICH device/protocol
dev_byte_xfer is driving (memcard write 0x57? controller? CD-stream?) and
whether the card RX=0xFF is a desync or expected NAK. The clean fifo_space_check
may also be mis-reading T2 because generated dev_byte_xfer and clean
fifo_space_check disagree on cycle-advance timing.

=== CORRECTION: dev_byte_xfer spin = PAD POLL, not memcard wedge ===
sio_trace shows the hot SIO traffic is dev=1 (DEV_PAD/controller),
mc_state=0 (MC_IDLE) throughout, func=0x00001B7C. tx pattern 0x00/0x01/0x42
+ rx 0xFF/0x41/0x5A = pad-poll protocol. mc_read_done frozen@108 is EXPECTED
(no card ops pending). => the 98M fifo_space_check spin is per-frame pad
polling throttled by the Timer2 wait — burns CPU but does NOT block a memcard
load and is NOT the +0x13 cause. Prior "memcard wedge" hypothesis WRONG.
REVISED MODEL: +0x13=0 is a game-LOGIC state, not an I/O stall. The game sits
in battle_intro_flow's internal do-while (state-6 path) which never calls
frame_advance_helper -> +0x13 frozen at entry value 0. Forcing +0x13=1 makes
gpu_draw_otag submit (gp0_draw +3510) but pixels stay black because the OT
prims are degenerate/garbage — the fighter/.ANI/.ACT scene geometry was never
decoded, so GTE vertices are invalid -> nothing rasterizes. The REAL blocker
is the fighter-model/animation load+decode RE backlog, upstream of rendering.

=== FULL CAUSAL CHAIN: black screen -> fighter file 0xb1 CD load stall ===
1. g_game_state=5/6 -> battle_intro_flow. DAT_1f800001=0x6c = polluted shared
   phase byte (no case) -> falls to frame_tick+round_banner = 2D-only black.
2. Forcing DAT_1f800001=0 -> phase SM advances 0->1, then STUCK at phase 1:
   needs menu_res_load_sm()!=0 AND _DAT_1f800090==0 to reach phase 2
   (battle_frame_tick = the 3D fight render).
3. menu_res_load_sm stuck at st20 -> save_load_state_machine @ s0=0x10 (L10)
   -> cd_file_load_sm(sel+0xb0). t1=DAT_1f800098=1 -> sel=1 -> file_id 0xb1.
4. filetab[0xb1] @0x801B611C = {0x801B58B4, 0x0F} -> filedesc sector records.
   cd_file_load_sm returns 0 every frame -> fighter .ANI/.ACT/model never
   loads -> DAT_1f8000e8/0x1f8001f0 stay 0 -> phase1 never completes.
5. CD-ROM delivery mismatch: cdrom_state bfrd_set=32866 vs bfrd_clear=13,
   ring_dropped climbing (13537), reading=0, pending cmd 0x09 inactive.
   The drive signals sector-ready ~32K times but the guest consumes ~13 ->
   the file-load sector counter never reaches N -> cd_file_load_sm=0 forever.
   decomp_trace shows OTHER files (0x50/0x54/0x55) DO load+LZSS-decompress,
   so the CD path basically works; the 0xb1 fighter file specifically stalls.
ROOT: CD-ROM sector-delivery/consumption fidelity — either the game's
ReadN->BFRD->DMA-drain handshake isn't completing for this file, or file
0xb1's extent/LBA resolves to sectors the drive delivers but the guest
never acknowledges. NEXT: trace cd_file_load_sm(0x8018cfdc) generated body
for the exact BFRD/status poll + DMA it waits on, and check whether file
0xb1's LBA is valid on the disc (vs the SetLoc[0,0,0] livelock the comment
documents). This is the faithful-timing/peripheral axis, not game logic.

=== GWATCH SCRATCHPAD EXTENSION + 0x6c WRITER ISOLATED ===
Built the missing instrumentation: guest_watch (mprotect page-protect) was
RAM-only (phys<2MB), so clean-C scratchpad stores were unwatchable. Extended
guest_watch.c to cover guest phys 0x1F800000-0x1F8003FF -> memory_get_
scratchpad_ptr(), and padded scratchpad[] to a page-aligned 16KiB alloc
(memory.c) so mprotect isolates it from .bss neighbours (no fault storm).
Now `gwatch addr=0x1f800001 len=1` catches EVERY scratchpad[1] store with
host rip + ra chain — including clean-C raw guest_at() stores that bypass
all psx_write_* traces (why wtrace showed total:0).

RESULT: the anomalous write is frame ~7967, old=0xf1 new=0x6c. The recorded
rip host-PC symbolizes (atos, __TEXT base 0x10425c000) to
battle_intro_flow/menu.c:~1964 = the `strb w28,[x8]` at vmaddr 0x10165789C
that writes DAT_1f800001=0xff (w28=0xff set once at fn entry). That store is
CORRECT — it wrote 0xff (exit phase). The recorded new=0x6c means a SECOND
clean store overwrote it inside the same pending window (page stays
unprotected between fault and guest_watch_block_tick, so merged writes keep
the FIRST writer's rip but show the LAST value). => DAT_1f800001 is a SHARED
phase byte raced by sequential state machines; the real 0x6c writer is a
clean store in battle_intro_flow's exit->return->re-dispatch window that
clobbers the byte before battle_intro_flow's next iteration reads it ->
default dead-loop (frame_tick+round_banner only, no battle_frame_tick).
NEXT: shrink the pending window (force guest_watch_block_tick sooner) or
log a second-chance rip, to name the merged 0x6c writer; then fix the
shared-phase-byte handoff faithfully (likely a missing DAT_1f800001=0 reset
on the state-6/battle entry path that the guest relies on).

=== STALL#3 (file 0xb1 / frame-348 codec race) — DMA PROGRESS + IRQ driver isolated ===
LIVE dma_trace (frames 345-346, just before the ~348 recursion trip) shows the
fighter-file load IS progressing: DMA ch3 (CD->RAM) completes transfers to
SEQUENTIAL buffers 0x800495DC->0x80049DDC->0x8004A5DC->0x8004ADDC (+0x800 each)
= the 0xb1 fighter payload landing. Two transfer shapes per frame: a 512-word
sector drain (bcr 0x10200) to 0x80049xxx and a 3-word control push (bcr 0x10003)
to 0x801BDCE8. So cd_file_load_sm is NOT dead — it is streaming the file.

THE RECURSION DRIVER: i_stat=0x00000004 (CDROM bit2) is asserted THROUGHOUT every
DMA op (i_stat_before==i_stat_after==0x4). The CD IRQ is LEVEL-triggered from
HINTSTS (cdrom.c: while irq_flag holds, i_stat bit2 re-asserts until the guest
writes 0x1F801803.1). During a streaming ReadN the drive raises a CD INT per
sector-buffer, so bit2 stays hot for the whole load.

MECHANISM (unifies stall#2 + file-0xb1): each CD IRQ -> game IRQ dispatch ->
cd_intc_dispatch (generated 0x8019a8ec) -> channel handler -> BIOS DeliverEvent
-> resumes the codec coroutine via a NESTED psx_dispatch_call (the clean<->gen
CPS boundary at br2_call_generated). The resumed coroutine's next MMIO/CD read
re-observes the still-asserted level i_stat bit2 -> re-delivers -> nests again.
Depth grows ~unbounded because the resume is nested INSIDE the not-yet-acked
IRQ window rather than deferred to the post-handler/outermost boundary.

cd_file_load_sm (gen 0x8018CFDC, 25-block CPS coroutine) yields FLAT via
cpu->pc continuations (does not grow depth). The depth growth is purely the
DeliverEvent/IRQ resume nesting at the br2_call_generated boundary.

FIX DIRECTION (faithful): the codec coroutine resume must be deferred until the
CD IRQ is ACKED (guest writes 0x1F801803.1 -> irq_flag clears -> i_stat bit2
deasserts) and the handler unwinds to an outermost/!in_exception boundary —
mirroring hardware where the woken thread runs AFTER the exception returns.
The s_defer_switch_* machinery (interrupts.c) already does this for
ChangeThread; the DeliverEvent/callback resume path needs the same defer-until-
acked treatment. NEXT: find the exact DeliverEvent->coroutine-resume call site
(does it go through psx_change_thread / g_sched_escape, or a direct
psx_dispatch_call of the event func?) and route it through the deferred path.

=== STALL#3 N+1 — PRECISE RECURSION MECHANISM (in_exception window) ===
IRQ take sets in_exception=1 + exception_nest_depth++ (interrupts.c:1560);
restored to prev at :2043 after the handler body. psx_check_interrupts gates
delivery on !in_exception. The codec coroutine (cd_file_read_seq, main-thread
sp=0x801FFFA0 — NOT a separate fiber) is resumed by the CD IRQ's DeliverEvent
through a NESTED psx_dispatch_call (br2_call_generated boundary), and that
resume executes AFTER in_exception has been restored to 0 but STILL INSIDE the
IRQ dispatch's host frames. So the resumed cd_reg_window_op MMIO sees
in_exception==0 + i_stat bit2 (level CD IRQ, still hot from the sector stream)
-> takes a FRESH nested IRQ -> handler -> DeliverEvent -> resume -> MMIO -> IRQ.
Each cycle adds a psx_dispatch_call frame that never unwinds -> depth 4097.

On hardware the woken thread runs AFTER the exception handler returns (RFE) —
the handler's stack is gone, so each IRQ is a fresh top-level delivery and the
coroutine's next MMIO IRQ is sequential, not nested. The recomp keeps the
resume nested inside the not-yet-unwound IRQ dispatch -> unbounded host depth.

FAITHFUL FIX: route the DeliverEvent/event-resume (and any in-exception thread
wake) through the scheduler escape — g_sched_escape / longjmp to
psx_scheduler_run — so the IRQ handler's dispatch frames UNWIND before the
codec coroutine runs, exactly like the s_defer_switch / ChangeThread path
(interrupts.c:2139-2192) already does for thread switches. The event-callback
resume must NOT run as a nested psx_dispatch_call inside the IRQ window.
NEXT: locate the LLE DeliverEvent->codec-resume call site (OpenBIOS
DeliverEvent/WaitEventFlag wakeup -> ChangeThread or direct func dispatch) and
confirm whether it bypasses g_sched_escape; if it uses psx_dispatch_call for
the woken context, switch it to the scheduler-escape path.

=== STALL#3 N+2 — TWO SEPARATE COROUTINES; recursion is the FILE-READ one ===
cd_ctx_restore (gen 0x8019ADCC, 60B) restores ra/gp/sp/fp/s0-s7 from the saved
ctx frame then `jr $ra` -> psx_restore_state_escape() = a LONGJMP that flattens
to the top-level dispatch. So the CD CHANNEL coroutine (cd_chan_full_init ->
cd_ctx_save -> on-IRQ cd_ctx_restore -> cd_intc_dispatch) resumes FLAT and does
NOT accumulate host frames — that path is correct.

The [NEST] recursion (psx_dispatch_call addr=0x8019B944 cd_reg_window_op,
ra=0x80156628 = cd_file_read_seq wait-loop callsite, sp=0x801FFFA0 main stack)
is therefore the FILE-READ/codec coroutine: cd_file_load_sm(0x8018CFDC) ->
cd_file_read_seq(0x801565ec) -> wait-loop calls cd_seek_complete_poll(0x8019D500)
-> ... -> clean cd_reg_window_op -> br2_call_generated(0x8019b944) ->
psx_dispatch_call [NEST+1]. Each nested call re-enters the SAME callsite with
identical sp/ra/pc => a suspended CPS continuation resumed via nested dispatch.

CONFIG TESTS (all crash identically at depth 256): PSX_DEFER_SWITCH=0 and
PSX_PRECISE_SLICE=1 — so it is NOT the ChangeThread deferred-switch path and NOT
a slice-timing race; it is a structural nested-dispatch resume.

NEXT: find what resumes the file-read coroutine's continuation inside the IRQ
window. Candidates: (a) DeliverEvent's event func callback (OpenEvent func),
(b) the cd_dma_chan_reset call_guest(@0x801bdee8) callback when non-NULL,
(c) a BIOS WaitEventFlag/TestEvent wake that dispatches the waiter via
psx_dispatch_call. Instrument psx_dispatch_call to also record the IMMEDIATE
caller (the clean fn that called br2_call_generated) via a caller-tag ring so
the resume site is named, not guessed.

=== SESSION: fight freeze ROOT-DECODED — 0xF0000009 = SPU/audio-stream-done ===
Live gwatch on 0x801BDF00 (armed before nav, whole run) + dma_state + handler
table dumps give the COMPLETE mechanism, superseding the "CD-ROM IRQ" framing:

EVENT: cd_seek_step -> cd_seek_complete_poll(0x8019d500) -> cd_seek_state_
machine(0x8019d5b0) = BIOS trampoline `li t2,0xB0; li t1,0x0B; jr t2` =
kernel TestEvent(handle @0x801bde38 = 0xF1000008 = ev[8]). Event opened in
cd_ext_init_once (0x8019b308) via FUN_8019b380(0xf0000009,0x20,0x2000,0)+
FUN_8019b390 EnableEvent. Live handle is a VALID EvCB 0xF1000008 (OpenEvent
worked; the session-18 0x8019B0FC corruption is NOT present this run).

DELIVERER CHAIN (all generated-verified):
- cd_cb7_run (0x8019af84) = DMA-interrupt dispatcher. Reads DICR via
  0x801BDDF0->0x1F8010F4, mask=(DICR>>24)&0x7f, calls per-channel done-handler
  from moving-cursor table @0x801BDDF4. Handler slots: ch3(CDROM)@0x801bde00
  =0 NULL, ch4(SPU)@0x801bde04 = 0x8019B7E0.
- 0x8019B7E0 (ch4/SPU done handler, size 0xBC): loads flag 0x801BDF00;
  bne flag,0 -> flag-set path does SPU-status dance on 0x1F801C00+0x1AA
  (0x1F801DAA, &0xFFCF) then DeliverEvent; flag==0 -> jal 0x8019BE78 (alt path).
- cd_dma_chan_reset label 0x8019b7e8 is a SUB-ENTRY inside 0x8019B7E0 — polls
  SPU reg 0x1F801DAA bits 0x30 then cd_bios_svc_07(0xf0000009,0x20).
- 0x1F801C00 = SPU register file (memory.c "SPU: 0x1F801C00..0x1F801FFF"),
  NOT CD-DMA. The earlier "CD-DMA regs 0x1F8010C0/C4/C8" = DMA ch4 = SPU.

FLAG 0x801BDF00 lifecycle (3 stores, all via base 0x801C0000-8448):
- ARM =1: 0x8019B928 inside br2eu_func_8019B89C = cd_dma_xfer_start (li v0,1;
  sw v0,-8448). Reached ONLY via cd_dma_xfer_mode(0x8019CFC0)<-audio_seq_reset
  (0x80158244, the state-9 CD handler). fntrace=0 hits all session.
- CLEAR =0: 0x8019B9E8 (sw $zero) inside br2eu_func_8019B954 status-poll, ran
  3x early (frames ~1142-1637) on already-0 flag = no-op.
- 0x8019BA54 stores gpr[6] (third store, never observed).
- GWATCH RESULT: flag written ONLY 3x whole run, ALL =0 clears early. The =1
  arm (0x8019B928) NEVER executed => 0xF0000009 NEVER delivered via flag path.

STATE-9 / AUDIO CHANNEL: 0x801BDEEC (cd_state9_store target) = current CD
state-9 value. chan_reset_register(0x801582c0) sets it to 0x80158244
(audio_seq_reset) at boot/intro (save_flow_state_machine s0==0). Live =0 at
freeze => audio/seq channel torn down. chan_reset_clear(0x80158358) sets
state9=0 + zeroes buffer 0x801f36f0 (shared s16-table scratch too) — called
from save_flow_state_machine s0==0xf0 (options/save screen teardown) and 2
other sites. audio_seq_reset = cd_reg_bit40_ctrl(0)+cd_dma_xfer_mode(
0x801f36f0,5) = the SPU stream kick.

DMA STATE at freeze: ch4(SPU) madr=0x4F6E0 bcr=0x0F930010 chcr=0x201
active=0; ch3(CDROM) active=0. No SPU transfer in flight. SPU status regs
0x1F801DA0-AAC all 0 (bits 0x30 clear => cd_dma_chan_reset poll would pass
immediately IF invoked).

cd_intc_dispatch (0x8019a8ec) handler table @0x801bcd1c populated:
slot0=0x8019ae68 slot2=0x801979f8 slot3=0x8019af7c, mask@0x801bcd48=0xd.
Clean-wrapper scratch counter @0x801bce00 =0 (only proves clean entry not
taken; generated may dispatch directly — ambiguous probe).

FILE-LOAD side (works): cd_read_log shows healthy CD-ROM ch3 DMA — fight load
read LBA8802 sectors ->0x8009C000..0x800A1800 (+0x800 each) with interleaved
size=12 header reads ->0x801BDCE8, THEN stalled spamming lba=8770 size=12
x32+ to 0x801BDCE8 (a poll that never satisfies). File data DOES land; the
stall is the TestEvent(0xF0000009) wait, i.e. the game couples CD-read
completion to SPU-stream drain.

ROOT CAUSE (refined): the fight's audio/SPU streaming channel is NOT active
(state-9=0, buffer cleared, no ch4 DMA) so the ch4-done->DeliverEvent(0xF
0000009) never fires and cd_seek_step's TestEvent blocks the game thread.
NEXT: find what should re-arm state-9=0x80158244 / start the fight music- seq
stream on fight entry (likely a skipped fight-init step, same class as the
versus fighter-data gap). Check whether chan_reset_clear ran during the
menu->fight transition and whether a corresponding play/seq-load should
follow. Alternative: the file-load's OWN completion may deliver 0xF0000009
via a cd_intc_dispatch channel handler (slots 0/2/3) — verify which handler
owns read-done and whether it runs.

## FIGHT-FREEZE ROOT CAUSE — OpenBIOS HookEntryInt semantic gap (SUPERSEDES audio theory)

The prior "SPU/audio channel torn down" theory was WRONG. Decisive decode of the
actual deferred-CD-interrupt contract (generated disasm + OpenBIOS source):

GAME MECHANISM (Sony-kernel style):
- cd_file_read_seq(0x801565ec) -> cd_seek_param_issue(0x8019D410) ->
  cd_seek_complete_poll(0x8019D500) -> cd_seek_state_machine = BIOS TestEvent
  on handle @0x801bde38 = event 0xF0000009 spec 0x20 (EvCB 0xF1000008, valid).
- cd_chan_full_init(0x8019A814) is the resumable coroutine half. On first entry
  (flag @0x801CCD18==0) it zeroes a state block, then block_8019A874 calls
  cd_ctx_save(0x801CCD50) which snapshots ra/sp/fp/s0-s7/gp (jmp_buf layout)
  and returns; continuation 0x8019A87C branches on v0: v0==0 -> arm path,
  v0!=0 -> block_8019A884 -> jal cd_intc_dispatch(0x8019A8EC) -> DeliverEvent.
- The resume is driven by B0/0x19 = Sony HookEntryInt: game calls
  setExceptionJmpBuf(frame); kernel stores frame ptr; on IRQ the kernel
  restores the jmp_buf (sets v0=1, jr ra=frame.JB_PC) -> lands at 0x8019A87C
  -> cd_intc_dispatch -> DeliverEvent(0xF0000009) -> TestEvent unblocks.

OPENBIOS (the actual BIOS — NOT retail SCPH-1001):
- B0table[0x18]=setDefaultExceptionJmpBuf, [0x19]=setExceptionJmpBuf.
  JmpBuf layout {ra,sp,s8/s8,fp,s0-s7,gp} MATCHES game cd_ctx_save frame.
- g_exceptionJmpBufPtr is honored ONLY on the UNHANDLED path: exceptionHandler
  (vectors.s) runs priority verifiers; IRQVerifier returns 0 (delivers
  OpenBIOS EVENT_* + acks as side-effects, does NOT take). If NO handler's
  verifier returns nonzero -> "nobody took the call" -> longjmp g_exceptionJmpBufPtr.
- OpenBIOS enqueues its OWN CD-ROM driver handlers (enqueueCDRomHandlers,
  statemachine.c) which DO take CD IRQs -> exception marked HANDLED ->
  returnFromException (TCB restore) -> the game's hooked frame is BYPASSED.

=> On a handled CD IRQ, OpenBIOS never restores g_exceptionJmpBufPtr, so the
game's deferred coroutine never resumes, cd_intc_dispatch never runs,
DeliverEvent(0xF0000009) never fires, TestEvent spins forever = freeze.

EVIDENCE: bioscall_dump — B0/0x19(HookEntryInt) x1, B0/0x18(ResetEntryInt) x1,
DeliverEvent x67 (none =0xF0000009), ReturnFromException x297079,
TestEvent x1,414,357 (the spin). Frame @0x801bcd50 armed; 0x801CCD50 frame
region holds state block. i_stat bit2 (CDROM IRQ) fired on last cmd (seq144).

FIX (faithful, rule -1): OpenBIOS exceptionHandler must honor
g_exceptionJmpBufPtr at the end of HANDLED exception/interrupt processing too
(Sony EntryInt = universal exception-exit hook), not only for unhandled.
Requires patching pcsx-redux OpenBIOS (vectors.s/handlers) + rebuild
openbios.bin (mipsel-none-elf toolchain, pinned 55fbf046) + regen OpenBIOS_full.c
via psxrecomp/tools/regen_bios.sh. NOT a per-game hack — it is the documented
kernel mechanism the game depends on; general for every game using HookEntryInt.

STATUS / APPLIED PATCH (pcsx-redux OpenBIOS @ /private/tmp/nugget, MIT):
The minimal faithful change is in cdrom/statemachine.c. On Sony, an IRQ
handler returns and the COMMON exception exit restores EntryInt; OpenBIOS
handlers instead call syscall_returnFromException() (B0/0x17 -> TCB restore),
which exits exceptionHandler via jr $k0; rfe BEFORE the g_exceptionJmpBufPtr
longjmp is reached. Removing that call makes the handler return normally so
exceptionHandler falls through to the universal g_exceptionJmpBufPtr restore.
The DEFAULT frame's .ra == returnFromException (kernel/handlers.c:119), so
un-hooked operation produces the IDENTICAL full TCB restore — only hooked
(HookEntryInt) frames change behaviour, which is exactly the Sony contract.

  cdromIOHandler  (statemachine.c ~705): dropped syscall_returnFromException()
                  after IREG = ~IRQ_CDROM  -> returns, hits jmp_buf restore.
  cdromDMAHandler (statemachine.c ~720): same for IREG = ~IRQ_DMA.
Both edits APPLIED in the local OpenBIOS source tree (not yet built).

NOTE: irq.c T0..T3handler (timer/VBLANK) call returnFromException() the same
way and carry the same latent divergence for OTHER IRQs during a hook window;
they are left unchanged because the proven freeze is the CD-ROM IRQ path and a
broad untested change is riskier. Audit them if residual stalls appear.

LANDING PATHS (pick one):
 A) Full SDK rebuild: brew tap pcsx-redux/mips -> mipsel-none-elf-gcc
    (formula builds gcc-16.2.0 from source, RUNNING in bg), then
    make -C openbios PREFIX=mipsel-none-elf FORMAT=elf32-littlemips
    (needs common/psxlibc + shell deps built for mipsel), then
    tools/regen_bios.sh --config bios/OpenBIOS.toml, rebuild runtime, test.
 B) Binary patch: locate cdromIOHandler/.ramtext in bios/openbios.bin
    (kernel .data copy ROM 0xBFC1E4D4 -> RAM 0x500, kernel_bless), NOP the
    returnFromException call, regen OpenBIOS_full.c. Faster but needs the
    openbios.elf symbol table (not in repo) or careful .ramtext disasm.
Toolchain build status: brew install pcsx-redux/mips/mipsel-none-elf-gcc in
progress (compiles binutils+gcc from source; deps gmp/mpfr/mpc via brew).

=== RESOLVED: fight-entry freeze (HookEntryInt) — VERIFIED 2026 (commit f019a17) ===
Path B (binary patch) landed. Located cdromIOHandler @0xBFC1E9E8 and
cdromDMAHandler @0xBFC1EA24 in bios/openbios.bin (.text, IREG=0xBF801070);
NOP'd the 3-word frame+jalr(returnFromException) prologue in each (6 words
total) so handled CD/DMA IRQs return normally -> exceptionHandler reaches
the universal g_exceptionJmpBufPtr restore (Sony EntryInt semantics).
Patched image sha256 6ad31beb...; OpenBIOS.toml hash updated; regen via
psxrecomp-bios --config bios/OpenBIOS.toml (EMIT OK 651 fns); runtime
rebuilt. VERIFIED live: nav_arcade reaches gs=8/ph=8; B0/0x0B TestEvent
count FROZEN at 1,593,985 (was spinning ~M/s); B0/0x07 DeliverEvent=136;
CD deferred coroutine resumes via jmp_buf (disp_ring: 0x8019A87C->
cd_intc_dispatch 0x8019A8EC->cd_cb8_dispatch 0x8019AE68->DeliverEvent);
DAT_801bdde0 advances. The original wedge is FIXED.

=== NEXT STALL (post-entry): fight renders but does not simulate ===
Game now reaches gs=8 -> case8 battle_flow_state_machine -> match_loop ->
frame_tick every frame (render driver; _DAT_1f8000dc advances ~432/s).
CD command queue EMPTY (head==tail @0x801b8448/44c) — not a load stall.
battle_intro_fsm phase _DAT_801f0738 == 3 (live-fight phase; would run
task_pools_tick(2)+task_slot_fns_run(1)+fighter updates). Fighter objects
@0x801f5258/0x801f52a0 POPULATED (real ptrs, 1356/4096 nonzero in
0x801f5000-0x801f7000) — NOT the versus empty-data bug. YET the entire
fighter region is byte-identical over seconds (0 diff) and screenshots
pixel-identical: battle_frame_tick's fighter updates are NOT landing.
Conclusion: frame_tick(render) runs but battle_frame/battle_frame_tick
(fighter SIM, 0x8014cdd0/0x8013xxxx) is either never called by
battle_flow_state_machine or its fighter-update path is gated off.
START input does not change it. NEXT: determine whether battle_frame is
reached (fntrace/clean-table hit on 0x8014cdd0) and what
battle_flow_state_machine (generated 0x8016fcc4) waits on before calling
it — likely a ready/loaded flag or a deferred event the coroutine should
deliver. Same event-delivery class as the fixed wedge; check whether the
specific completion event the battle waits for is being DeliverEvent'd.

=== POST-ENTRY STALL — refined root-cause narrowing (verified live) ===
Ruled OUT (all checked live on the running gs=8/ph=8 fight):
- CD command queue: EMPTY (head==tail @0x801b8448/44c). Not a load stall.
- Asset decompress: decomp_trace ring FROZEN at total=4134 (LZSS/file_load
  stream completed; dst marched 0x800A3F14->0x800E1064 then stopped).
- VBLANK: cycle_paced_raise=362170, delivered=362000 — vsync IRQs fire 60Hz.
- Fighter data: structs @0x1f8000e8/0x1f8001f0 POPULATED (real char-data
  ptrs 0x8006ba58/0x800ce9ac, anim tables 0x801beff0/0x801bf860, 0x801c00d0).
- Sim gate: _DAT_1f800082==0 so case8 DOES call fighter_contact_resolve,
  fighter_pair_update, battle_camera_step, AND fighter_frame (FUN_801787dc)
  once per fighter every frame.
- Input: press START (buttons=8) does not change gs/ph or fighter state.

CONFIRMED STALL: fighter_frame runs per frame but fighter STATE structs
(0x1f8000e8/1f0, 0x108 stride) AND the rendered frame are byte-identical
over seconds — the per-fighter ANIMATION/ACTION frame is not advancing.
fighter_frame writes timers/state (param_1+0xd6/0xd8/0xaa) that stay fixed,
meaning its action/anim-advance branch is not progressing — the .ACT/.ANI
action VM or anim-frame counter is not stepping. NEXT: find the per-fighter
anim-frame/action-timer field fighter_frame increments (or the ev_stream/
action_switch path) and determine what global tick it depends on — likely a
per-battle-frame counter or the event/VM dispatch that should run each
battle_frame but is gated. Check whether battle_frame (0x8014cdd0, the OTHER
battle driver distinct from battle_flow_state_machine) is the one that ticks
the action VM and whether IT is reached.

=== ARCHITECTURE FINDING: battle runs GENERATED MIPS, not clean-C ===
The live main loop is generated SLES_017.22 code. block_8012665C does
cpu->pc=0x8016FCC4 (JAL battle_flow_state_machine); ra=0x80126664 is its
return site. disp_ring ra=0x80126664 confirms battle_flow_state_machine
IS entered and issues the CD/GPU/vsync calls seen in the ring. HOWEVER
the CLEAN-C battle_flow_state_machine (menu.c:549) is NOT the executed
path — its decomp_trace tag-60 (logs DAT_1f800001 every call) NEVER
appears and the decomp ring is frozen at total=4134. So 0x8016FCC4 runs
GENERATED MIPS (faithful recompile), not the clean-C port. Consequence:
the fight-sim stall is a RUNTIME/data condition inside generated code,
NOT a clean-C logic bug. ALSO relevant to the "clean C actually used"
goal: battle_flow_state_machine's clean port exists but is bypassed —
verify whether wrap_8016fcc4 in clean_table is active or whether the
generated body shadows it. NEXT: find the generated battle_flow_state_
machine's poll/wait condition at ph=8 (it loops CD/GPU/vsync without
advancing fighter anim — likely a resource-ready or event flag).

=== CORRECTION + ROOT CAUSE of fight-sim stall (session cont.) ===
Earlier "battle runs generated / ph=8 sim" conclusion was WRONG. cyc_watch
(verified working via cd_timer_read hits) shows battle_flow_state_machine
entry block 0x8016FD38, case-8 handler 0x80170260, AND gs=8 case blocks
0x80126654/5C/64 ALL get 0 hits — battle_flow is NEVER entered. The gs=8
case calls match_mode_set(0x801560F8) then JALs battle_flow; neither runs.

REAL stall: the game is spinning in cd_seek_complete_poll (0x8019d500,
delegated to generated) -> cd_seek_state_machine (0x8019d5b0) = a leaf
TestEvent trampoline polling event flag handle 0xF1000008 (= OpenEvent
class 0xf0000009 spec 0x20 stored @0x801bde38). bios_svc_trace: ~all
recent calls are B0/0x0B TestEvent a0=0xF1000008, v0=0 forever.

Evidence:
- bioscall_dump: DeliverEvent (B0/0x07) count FROZEN at 136 (was the
  fight-entry load deliveries). TestEvent B0/0x0B = 1,593,985.
- cdrom_command_history: LAST CD command seq224 = cmd 0x09 Pause at
  frame ~2491; current frame ~765049 -> NO CD command issued for ~762k
  frames. Repeating SetLoc(0x02)->ReadN(0x06)->Pause(0x09) loop then STOP.
- cdrom_state frozen: seq=119859, reading=0, pending cmd0x09 active=0,
  seek_msf[1,58,70] vs read_msf[1,59,27], bfrd_set=6800 vs bfrd_clear=13,
  ring_dropped=2874, i_stat=0, irq pending=0.
- irq_state: i_mask=0xD (CD unmasked), i_stat=0, pending=0.

Interpretation: the game finished initial asset loads (136 DeliverEvents),
issued a final SetLoc->ReadN->Pause at frame ~2491, then blocked waiting
for a CD completion event that requires a NEW DeliverEvent — but no new
CD command is issued and no CD IRQ is pending, so DeliverEvent never
fires and TestEvent spins forever. Either the last ReadN was interrupted
by Pause before its data-end/completion delivered, or the completion
DeliverEvent was consumed/lost. The bfrd_set:6800 vs clear:13 + dropped
sectors suggest the sector stream/consumer is imbalanced. NEXT: determine
whether the game is waiting for a ReadN data-end (INT1) that the Pause
preempted, and whether the deferred CD coroutine / DeliverEvent path for
the read-complete event is reachable — compare against a working load
(e.g. does a fresh CD read after this point deliver?).

=== DEEP DIVE: CD completion->DeliverEvent chain (session cont.) ===
Confirmed the stall is NOT a logic freeze — it is a CD-ROM event-delivery
wedge. Precise chain:

WAIT: game spins in cd_file_read_seq(0x801565EC) -> cd_seek_complete_poll
(0x8019d500) -> cd_seek_state_machine(0x8019d5b0) = leaf TestEvent trampoline
polling event-flag handle 0xF1000008 (= OpenEvent class 0xF0000009 spec 0x20,
EvCB entry i=8 status=0x2000 mode=0x2000 fhandler=0). bios_svc_trace: ~all
calls B0/0x0B TestEvent a0=0xF1000008 v0=0 forever (~1.6M).

DELIVER side: DeliverEvent(0xF0000009,0x20) is issued ONLY by
cd_dma_chan_reset (game/cd.c:5089) when callback @0x801bdee8==0 (it IS 0 ->
the DeliverEvent branch is armed). cd_dma_chan_reset is reached via the CD
IRQ -> deferred coroutine (cd_ctx_restore -> cd_intc_dispatch -> cd_cb7_run
-> per-channel handler) -> channel reset. bios_svc B0/0x07 DeliverEvent count
FROZEN at 136 -> this chain has not run for the stuck completion.

DRIVE state (cdrom_state): reading=0, irq_flag=0, pending.cmd=0x09
active=0, stat=0x02 (MOTOR only, no READ/SEEK/PLAY/ERROR), i_stat=0,
irq_enable=0x07, seek_msf[1,58,70] vs read_msf[1,59,27],
sector_available=1 (one sector unconsumed), bfrd_set=6800 vs bfrd_clear=13,
ring_dropped=2874. cycles_to_next_event cdrom=0xFFFFFFFF -> NOTHING scheduled.
irqctx_ring: only VBLANK (istat=0x1) IRQs taken; cdrom_active=0 always.

COMMAND history (sorted): game cycles SetLoc(0x02)->ReadN(0x06)->Pause(0x09);
each Pause's INT2 (~1.1Mcy) is cancelled by the next SetLoc (psx-spx command
cancellation, cdrom.c:2141). LAST cmds: seq339 ReadN@frame4590 (reading=1),
seq340 Pause@frame4601 pending=1 -> then NO commands for ~760k frames.
ReadN has no completion IRQ of its own (INT3 ACK only; streams INT1 sectors).

INTERPRETATION: the game finished a read chunk and entered cd_seek_complete_
poll waiting for the completion DeliverEvent. The drive is fully idle — no
command in flight, no IRQ owed, nothing scheduled — so no CD IRQ will ever
fire to drive cd_ctx_restore->cd_intc_dispatch->cd_dma_chan_reset->DeliverEvent.
Either (a) the seq340 Pause/read-end completion fired an IRQ whose
DeliverEvent was lost (handler ran but didn't reach cd_dma_chan_reset, or the
IRQ was acked without the coroutine resume), or (b) the game is waiting for a
completion of an operation queued in its CD driver but never issued to
hardware because the issuing pump is itself gated on the same event.
NEXT: instrument cd_dma_chan_reset (0x8019b7e8) + the game's CD IRQ handler
entry + cd_intc_dispatch to see whether the completion IRQ is raised at all
for seq340, and whether a CD command sits queued-but-unissued in the game's
driver (cd_cmd_queue / pending_args). Compare a WORKING file load (early
boot, DeliverEvent->136) vs this fight-asset load for the divergent step.

=== wtrace CORRECTION: DeliverEvent/TestEvent mechanism WORKS (session cont.) ===
Earlier hypothesis "LLE BIOS DeliverEvent does not mark EvCB slot8 READY" is
DISPROVEN. Armed wtrace on slot8 EvCB (0x8EE8..0x8F00) across a fresh
nav_arcade run to the gs=8/ph=8 wedge. Captured writes to status word 0x8EEC:
  seq205/211/213  0x2000->0x4000  pc=0xBFC20678 (BIOS) ra=0x8019B88C  = DeliverEvent
  seq206/212/214  0x4000->0x2000  pc=0xBFC0577C (BIOS) ra=0x8019D550  = TestEvent consume
So OpenBIOS DeliverEvent(0xF0000009,0x20) DOES set slot8 READY and TestEvent
DOES consume it back to ENABLED and return success — 3 full cycles observed.
The bios_svc_trace v0=0 samples are the SUBSEQUENT spin after the last delivery.

=> The event/delivery path (ch4 DMA done -> cd_cb7_run -> cd_dma_chan_reset ->
   jr 0xB0 DeliverEvent -> slot8 READY -> TestEvent) is HEALTHY.

REAL STALL (unchanged, now precisely bounded): after the last consumed event
the game issues/awaits ANOTHER completion whose DeliverEvent never arrives.
cd_dma_chan_reset ran only 4x total; drive went fully idle
(cdrom_state: reading=0, pending.active=0, i_stat=0, irq_flag=0,
request=0x80 BFRD still armed, sector_available=1 pos=2060/2340,
ring_dropped=743, stat=0x02 MOTOR-only). ch4 idle chcr=0x201, DICR no done bits.
The awaited op is the ch4/SPU (fight audio XA stream) completion — DeliverEvent
0xF0000009 is the SPU-DMA-done signal, not a generic CD-read-done.

NEXT (unchanged from prior): determine whether a CD command sits
queued-but-unissued in the game driver (its issue pump gated on the same
event), or whether the XA-audio/SPU stream stopped because sectors stopped
being classified/delivered as audio (maybe_deliver_xa_audio / xa_data_end).
Sector pos stuck at 2060/2340 with BFRD armed suggests ch3 CD-ROM DMA also
stalled mid-sector — check whether ch3 (CD-ROM DMA ch) has an unserviced
transfer or whether cdrom_dma_ready is being starved.

=== ch3 CD-ROM DMA stream STOPS at lba 8801 ~frame1688 (session cont.) ===
dma_cdrom_history (2013 entries) shows the game streamed CD sectors via ch3
only up to lba 8801 — every transfer frame_start=frame_end in 1686..1688 —
then ZERO ch3 reads for ~43000 frames while wedged at gs=8/ph=8. Per lba the
game issues a 3-word header read (pos 0->12) + a 512-word data read
(pos 12->2060 of sector_size 2340; the 280-byte tail is subheader/ECC, normal).
Last sector lba8801 seq2012 last_words are all 0x00000000 (tail read hit an
empty/zero region). request=0x80 BFRD still armed, sector_available=1.

=> The sector stream legitimately ENDED (drive reading=0, no pending cmd, no
INT1 pending/lost) around lba 8801 — either the ReadN range was exhausted or
an XA EOF submode (0x80|AUDIO -> xa_data_end_pending -> stop_read_stream,
cdrom.c:1495) fired. The game then sits in cd_seek_complete_poll->TestEvent
waiting for the ch4/SPU-DMA DeliverEvent that would accompany the NEXT audio
buffer, but issuing that next read/transfer is gated behind the same wait —
circular stall. The break is therefore NOT event delivery (proven healthy)
but that the audio/file stream ended without the game receiving the
continuation it expects: either (a) ReadN covered too few sectors / wrong LBA
range so the stream truncated mid-file, or (b) the XA data-end/EOF INT that
should tell the game "stream finished, wrap/continue" was consumed without
the matching game-side handler advancing.
NEXT: read the game's CD audio-stream state machine (audio_seq_reset /
cd_reg_stream_push / the ReadN sector-count bookkeeping around the last lba)
to learn whether it expected more sectors past 8801, and whether an
xa_data_end/cdda_data_end INT was raised for lba8801.

=== SPU/audio pipeline NEVER started this session (session cont.) ===
spu_status: active_mask=0x000000 (no voices), render_frames=107M but
nonzero_frames=0 (total silence), cd_l/cd_r/main_l/main_r all 0.
audio_stats: cd_in frames=0 — CD audio never reached SPU.
spu_events: only 48 events TOTAL, ALL at frame~318 (boot voice KEYON/KEYOFF
v0-23); ZERO SPU voice/regs commands since boot. key_on_count=24 historical.
=> The fight audio driver never ran: game is blocked BEFORE audio init,
waiting on a ch4/SPU-DMA DeliverEvent for a transfer it has not yet kicked
(the sound-bank/stream upload), and that kick is gated behind the same
TestEvent wait -> circular stall, now confirmed at the audio-driver level.
The ch4 transfer the game awaits is almost certainly the sound-data upload
into SPU RAM (not live voice streaming). NEXT: trace the game's sound-init /
audio_seq_reset / cd_reg_stream_push path to find the ch4 kick that should
precede this TestEvent, and why it isn't issued (likely gated on a prior
CD-read completion that itself is the stalled event).

=== MECHANISM FULLY VERIFIED; stall = sequencing at fighter-record read ===
spu_ta ring (PSX_SPUDTA) captured the register-window handshake live:
per cd_seek_param_issue -> op2 writes TA(0x1F801DA6)=shadow, op1 polls TA
(match) then writes SPUCNT 0xC001->0xC021, op3 polls (SPUCNT&0x30)==0x20 then
writes ch4 MADR/BCR/CHCR=0x01000201 (start). On ch4 done the SPUCNT bits clear
back to 0xC001 (cd_dma_chan_reset) and DeliverEvent fires. THREE handshakes
seen at cyc ~789M/974M/1005M; LAST at ~cyc1008M, then ZERO further SPU-reg or
ch4 DMA activity (event_ring 65k window: only ch2/ch6 GPU, no ch3/ch4).
cd_reg_window_op op3 disasm confirms it computes file offset/count -> ch4
MADR/BCR and kicks CHCR=0x01000201. Handler table @0x801bddf4: ONLY ch4(SPU)
has a handler (0x8019b7e0->DeliverEvent 0xF0000009/0x20); ch3 CD-ROM none.

Caller chain: fighter_rec_fetch_apply -> fighter_record_build ->
cd_file_read_seq(0x801565ec) -> cd_seek_param_issue_clamped ->
cd_seek_param_issue -> [register-window op2/1/3 -> ch4 kick] ->
cd_seek_complete_poll(0) -> cd_seek_state_machine -> TestEvent(0xF1000008).
=> The game is loading a FIGHTER RECORD file through the SPU/ch4 path and is
blocked in the completion poll. It already kicked ch4 for the reads that
completed (3 events consumed) but is now waiting for a completion whose next
ch4 kick never issued — i.e. it is parked in cd_seek_complete_poll AFTER
cd_seek_param_issue already ran, so THIS read's ch4 either (a) completed and
its DeliverEvent was consumed by a prior/overlapping TestEvent on the shared
slot8 flag, or (b) the read is sector-starved: fighter file needs more sectors
than the drive delivered (ring_dropped) so the SPU never finishes and the
next chunk's handshake never runs. The HLE thread scheduler runs the CD IRQ
deferred coroutine (cd_ctx_restore->cd_chan_full_init->cd_intc_dispatch) on a
separate context that shares event slot8 with the main thread's TestEvent —
a candidate for the consume-mismatch. NEXT: determine whether the current
cd_file_read_seq already issued its ch4 (check 0x801bdee4 result + whether a
4th handshake is pending vs consumed), and whether the fighter file's sector
count vs delivered count diverges (ring_dropped correlation).

=== slot8 LIVE-VERIFIED ENABLED+MATCHING; DeliverEvent simply not re-firing ===
Live EvCB dump this run: slot8 @0x8EE8 cls=0xF0000009 st=0x2000(ENABLED)
spec=0x20 mode=0x2000(MARK) fn=0 — a DeliverEvent(0xF0000009,0x20) NOW would
set it READY. hle_dump DeliverEvent history: 0xF0000009/0x20 fired seq2133
cyc236M (rep1) and seq2420 cyc791961513->cycle_last1008314781 (rep3 = the 3
spaced handshakes ~792/974/1008M, NOT coalesced). Last SPU-reg access
spu_ta seq59 cyc1008314735 — no 4th handshake since.
=> The current cd_file_read_seq poll needs ONE more DeliverEvent but no new
ch4 completion is produced: either (a) this read's ch4 was kicked+completed
and its DeliverEvent found slot8 NOT-enabled at that instant (event-lifecycle
desync: game Disable/Close/Enable ordering vs async IRQ-context DeliverEvent),
or (b) op3's ch4 kick for this read silently failed (SPUCNT poll timeout ->
no CHCR write -> no transfer -> no DeliverEvent -> TestEvent spins), or
(c) the read is sector-starved and the SPU never signals done.
slot8 st=0x2000 ENABLED now proves the flag is armed and waiting — the miss
is upstream in producing the completion, not in event delivery.
NEXT (decisive): wtrace slot8 status (0x8EEC) + correlate each write's
pc/ra/cycle against hle_dump DeliverEvent seqs and spu_ta handshake seqs on a
fresh run — determines whether the last DeliverEvent found slot8 disabled
(write skipped) vs whether a 4th ch4 kick is simply absent.

=== SESSION UPDATE: fighter SPAWN fixed via clean-cycle accounting; versus->fight crash PROVEN clean-independent ===
ROOT CAUSE of the earlier "fighters never spawn / fobj=0" wedge: clean-C
dispatch (clean_try_dispatch -> wrap_*) ran with ZERO guest-cycle accounting
while generated bodies self-charge via psx_cyc_step (~1 base + load
interlock). Clean hot paths therefore advanced psx_cycle_count far too
slowly, so CD-load/IRQ/SPU deadlines never landed inside clean waits and
fighter-model bring-up stalled before spawn.
FIX (commit d8a3445): gen_clean_table.py now emits a per-CleanDispatchEnt
`cost` derived from the generated oracle's per-function byte size
(instr_count * ~2 CPI) and clean_try_dispatch calls psx_advance_cycles(cost)
after each clean call. Verified live: versus->fight now spawns BOTH fighters
(state=0x1, valid fobj/parts ptrs, sane +/-256 spawn x, phase advances).
NOTE: psx_instr_base_cycles()==1 is the documented per-instr base; the flat
~2 CPI is a lump-sum approximation (decode-based n_instr+n_loads*K gives a
similar magnitude, so it is roughly calibrated, not the crash cause).

versus->fight crash is PRE-EXISTING + CLEAN-INDEPENDENT (A/B proven):
pure-generated (CLEAN_RANGES="0x0-0x0", no clean code) crashes identically
pc=0x0 in the OpenBIOS kernel spin, just later (frame ~12541 vs clean ~5527)
and at a different EPC (0x8018BD7C vs 0x8019A648). Both end in the same
low-memory kernel loop (last_func_addr=0x000029CC) after the B0 kernel
trampoline (jr 0xB0, t1=fn). => NOT a clean-port bug; it is the runtime's
OpenBIOS-kernel / IRQ / CD-DMA event path.
NEW crash manifestation observed: "DISPATCH FATAL: misaligned target
0x00030001" with COP0_EPC=0x8019A648 — inside the vsync_wait_n spin reading
DAT_801bdde0. During the spin a VBlank IRQ fires and an indirect jump lands
on 0x30001, a DATA value (0x30000/0x30001 pairs populate struct arrays at
0x801B2Exx / 0x8008xxxx / 0x801B1xxx). So the IRQ/callback path executes a
data word as a code pointer -> pc=0. Consistent with the prior slot8
TestEvent-spin analysis: same versus->fight fighter-file SPU/ch4 completion
wedge, different surface symptom.
NEXT (unchanged, decisive): on a fresh run wtrace slot8 status (0x8EEC) +
correlate DeliverEvent seqs vs spu_ta handshake seqs to learn whether the
4th ch4 kick is absent or its DeliverEvent found slot8 disabled; ALSO catch
the misaligned-jump source PC (which jr/jalr loaded 0x30001) via cyc_watch
armed on the IRQ entry.

=== N+31 — versus->fight IN-FIGHT wedge: decisive data = ch4/SPU stream kick ABSENT ===
Now past fighter spawn (gs=8 ph=8, valid .ANI both fighters). Wedge moved to
the IN-FIGHT CD/SPU streaming completion: cd_seek_complete_poll spins
TestEvent(slot8=F0000009/0x20) forever. Live ground truth on wedged proc:
- dma_state: ch4 (SPU stream) madr=0xDDE60 bcr=0x03490010 chcr=0x201
  active=0 remaining=0 -> CONFIGURED but NEVER KICKED (no start bit).
  ch3 (CD data) also idle. Only ch2 (GPU OT) active.
- dicr=0x00900000 = bit20(ch4 IRQ en)+bit23(master) -> game DID arm ch4 IRQ.
- spu_ta total=0 entries=[] -> SPU stream path never transferred all session.
- cdrom_state: last CD cmd Pause(0x09) issued+completed frame2313; IRQ type3
  processed (cd_response_process pc 0x8019638C/8C/A8 ran); then CD silent.
  resp_count=1 sector_available=1 (partial 2060/2340) irq_flag=0 i_stat=0.
- irqctx: VBlank(0x1)+CDROM(0x4) IRQs still taken every frame at valid
  resume PCs; DMA IRQ(0x8) ceased ~frame1716 (unrelated GPU/other ch).
=> CONFIRMS doc hypothesis (b): the stream completion is never PRODUCED
   because the ch4/SPU DMA kick never fires upstream — NOT a dropped
   DeliverEvent (slot8 ENABLED+matching would accept it). The game's custom
   streaming engine (register block via ptr @0x801bdeb0, channel-4 SPU
   stream + channel-3 CD data) gates the ch4 kick on a condition that is
   never met in-fight. cd_chan_reg_init arms 0x801bdee4=1 dee8=0 deec=0;
   deec is the completion flag cd_state9_store sets on finish.
NEXT (decisive): find the ch4 kick site + its gate. The kick writes CHCR
start for channel4; trace which guest fn issues it (dma_reg_write pc for
ch4 CHCR 0x1F8010D8) and what SPU/status poll (SPUCNT / blk+0xd7 &0x7ff
idle wait in cd_chan_reg_init) precedes it — determine whether the gate
reads a status bit our SPU/CD emulation never sets, or whether the kick
code path is unreached. Compare vs oracle at same point.

=== N+32 — wedge narrowed to missing ch4/SPU-stream KICK (not a dropped event) ===
Live-verified on wedged proc (port 4370, frame frozen 2319):
- DeliverEvent(F0000009/0x20)->slot8 is produced ONLY by DMA channel-4
  completion: cd_cb7_run walks DICR donemask -> handler[ch]; live handler
  table @0x801bddf4 has ONLY handler[4]@0x801bde04 = 0x8019B7E0
  (cd_dma_chan_reset -> cd_bios_svc_07 DeliverEvent). ch3 CD-data has
  handler[3]=0 (handled via CDROM IRQ -> cd_response_pump instead). So the
  event REQUIRES a ch4 DMA completion.
- ch4 state: DPCR-enabled (channel_enabled(4)=bit19 of 0x3B3BBBBB=1),
  DICR IRQ-armed (dicr=0x00900000 bit20=ch4 en + bit23 master), regs
  programmed madr=0xDDE60 bcr=0x03490010 chcr=0x201 (dir+sync, NO start
  bit24). execute_ch4_spu would move payload then schedule_delayed_complete
  (~13k words * 48cyc) -> complete_transfer -> DICR bit28 -> DMA IRQ.
  dma_advance DOES drive advance_delayed_complete for all ch incl ch4, so a
  kicked ch4 WOULD complete. It never did: spu_ta total=0, active=0,
  dicr done-bits=0.
- => ch4 CHCR start bit is NEVER written: the game's channel-4 kick never
  fires. NOT a runtime DMA bug (kick->complete->IRQ path is sound) and NOT
  a dropped DeliverEvent (slot8 ENABLED+matching would accept it).
- cd_reg_window_op diag counters (0x801bce30..44): op3 calls=8, op3 -2
  poll-timeouts=0, last op3 ret=0 SUCCESS -> the ch4 kick is NOT skipped by
  the CD/SPU status poll timeout (hypothesis b disproven for this wedge).
- cd_chan_cmd_submit diag (0x801bcf00..94): total calls=554, ch3-arms=554,
  ch3-kick-reached=554, last channel arg=3, last CHCR=0x11000000 -> ALL
  submits are channel 3 (CD data). Channel 4 is NEVER submitted via
  cd_chan_cmd_submit -> it uses a DIFFERENT kick path (SPU driver / dedicated
  stream submission / BIOS SPU DMA), still to be located.
NEXT (decisive): find the ch4 kick site — the code that writes ch4 CHCR
(0x1F8010C8) start bit or calls the SPU/stream DMA submit. It is gated on a
ready/status condition upstream in the game's streaming engine (analogous
to the *(0x801bcc40)&0x40 wait that gates ch3 in cd_chan_cmd_submit). Trace
the in-fight streaming setup: which fn arms ch4 regs (the madr/bcr/chcr=0x201
write DID happen) vs which fn should add the start bit, and what status bit
that second write waits on. Compare vs oracle at versus->fight entry.

=== N+33 — ch4 kick ROOT CAUSE found: SPUCNT bits4-5 handshake poll ===
Debugger agent completed static+live diagnosis (no source edits):
- Kick site = tail of generated br2eu_func_8019B954 (cd_reg_window_op
  @0x8019B944 dispatcher). op-2 polls u16 @0x1F801DAA (SPUCNT) &0x30 for
  0x20 (state@0x801BDF00!=1) or 0x30 (==1), cap 0xF01; timeout -> -2 ->
  SKIPS MADR/BCR/CHCR writes -> ch4 never kicks -> no DeliverEvent ->
  cd_seek_complete_poll TestEvent(F0000009/0x20) spins forever.
- op-1 writes (old&0xFFCF)|0x20 (bit5); bit4 (0x10) must come from
  SPU-side/transfer state. Our runtime has NO SPU CPU: spu.c models voice
  regs only, spu_dma_ready()=1, spu_ta.total=0 -> 0x30 never appears.
- Pointer cells verified live: 0x801BDEB0->0x1F801C00(SPU blk),
  B4->0x1F8010C0(MADR), B8->0x1F8010C4(BCR), BC->0x1F8010C8(CHCR),
  DF0->0x1F8010F4(DICR), 0x801BDE04->0x8019B7E0(cd_dma_chan_reset=handler[4]).
- cd_dma_xfer_start @0x8019B89C is the OTHER kick path (no SPUCNT poll) —
  explains the few DeliverEvents seen; streaming-chunk pump is op-2.
- FIX SITE: psxrecomp/runtime/src/spu.c (+dma.c MMIO) — model SPU-side
  streaming state so SPUCNT 0x30 reflects real transfer/CPU state.
  Agent df54025e assigned.
- CAVEAT: same race can strike EARLIER — a fresh run wedged at init with
  i_mask=0x9 (CDROM IRQ bit2 disabled), cd_reg_window_op never called.

=== N+34 — port wave 3 committed (7d2020c + cb6dc32) ===
- bind.c: ~20 weak stubs -> clean C (GTE vec3 normalize/mag2 preserving
  $v0=MAC1+2+3 contract, BIOS svc trampolines B0:0x08/0x0C/0x4C/0x56/0x57/
  0x32-36/0x43, A0:0x72/0xAC, DI/EI wrappers, CD seq fns)
- save.c: op_flag_fn_check, cd_channel_init, ret_zero_d expander
- gfx.c: gpu_mode_regs_apply, gte_screen_init_defer, gpu_state_patch_defer
- menu.c: heap_serialize_tag_01000000; battle.c: jalr fn_lookup fallbacks
  (unported targets now call_guest/br2_call_generated instead of skip)
- cd.c: cd_intc_busy_read, cd_init_done_cb leaf ports
- clean_table: 1200 wrappers (was 1195), 82 skipped

=== N+35 — pc=0 early wedge: NOT a wave-3 regression (probably) ===
A/B test: pre-wave3 binary (f9eb255) reached gs=6 ph=240 (demo fight
scene) then hit the SAME pc=0 kernel-trampoline crash (frame frozen
12823, "jr 0xB0 t1=fn" null-fn signature from N+31). Wave-3 builds
(3 runs) died earlier at gs=0/pc=0 — consistent with the N+33 caveat
that the streaming/IRQ race can strike at multiple waits, but sample
size is small; if wave-3 builds keep dying pre-title after the SPU fix
lands, re-bisect {save.c,gfx.c,menu.c,battle.c} next.
Last svc before wedge: B0:0x56 ra=0x80126260 (game main state machine).

=== N+36 — CD read-completion handshake FIXED + two clean-port address typos ===
- cd_init_done_cb (0x80198590) stored to wrong guest word: generated does
  s0=0x801BCBA8, delay-slot addiu s0,-40 -> 0x801BCB80, then sw $zero,0x24(s0)
  = 0x801BCBA4 (the read-session busy flag cd_init_session/cd_read_poll
  handshake on). Clean port wrote 0x801BFFFC -> bcba4 stuck 1 -> infinite
  37-sector read retry. FIXED (b4c25f3). Verified: bcba4=0 post-fix vs =1
  on wedged builds.
- save_load_state_machine (0x80170FE0) bound the channel-script/primitive
  context record to guest_at(0x801f4a8)=0x0801F4A8 (7-digit typo) instead of
  0x801F04A8. All channel-pointer binds went to invalid low mem ->
  *(0x801F04A8)=0 -> scheduler 0x801343BC -> prim_task_run(0) walked addr 0
  -> task_invoke(0x01000000) fail-fast @frame1868. FIXED (ac8de0c).
- RESULT: game now boots clean and runs ATTRACT fight demo (gs=6) stable,
  frame 21000+ (~3.6min game time). First time past the CD wedge AND the
  early crash. versus->fight nav agent assigned (port 4680).
- OPEN: audit agent hunting more wrong-address typos (same bug class).

=== N+37 — missing-zero scratchpad typo sweep (same bug class) ===
Systematic grep for 7-hex-digit guest literals found a cluster of
"dropped a 0" typos, all verified against generated/sibling usage:
- save.c mc_slot_action + chan_blocks_reset callers: 0x1f800e8 ->
  0x1f8000e8 (side-A channel block; side-B 0x1f8001f0 already correct)
- menu.c text-prim/GPU/state blocks: 0x1f803xx -> 0x1f8003xx
  (0x1f80384/8c/f0/7c). Sibling code already used 0x1f80038c/3a8 for the
  same descriptor -> confirms base. print_mode_dispatch flag verified vs
  generated br2eu_func_8012CE30 (lui 0x1F80; load +0x384).
- menu.c announce/champion flag: 0x1f80dc -> 0x1f8000dc (mode_name_
  announce 0x80163634: lui 0x1F80; load +0xDC).
Commits: 8c4e960, 49b6e49, 029f21b. Audit agent continuing the
valid-but-wrong kernel-address sweep (0x801f4a8 class).

=== N+103 — clean_table thunk-skip breakdown + unwrapped named census ===
Regen at HEAD gives 1205 wrappers / 76 skipped (all "no-def" per generator,
but the real cause is the br2_call_generated body-scan at gen_clean_table.py:
125-130). Census of every fnmap addr lacking a wrapper:
- 64 SELF-DELEGATING thunks — body is/ends-in br2_call_generated(own addr).
  MUST stay unwrapped: clean_try_dispatch -> clean fn -> br2_call_generated
  -> psx_dispatch_call -> dirty_ram_dispatch -> psx_dispatch_game_compiled ->
  clean_try_dispatch(same addr) = infinite recursion. Includes all cd_*
  interrupt-bound delegates (correct per AGENTS.md §4) + scriptop_chk,
  mc_date_serial_cmp, impact_fx_update (0x80138CEC tail-self-call).
- ~10 REAL clean bodies that merely CONTAIN br2_call_generated(variable) for
  dynamic sub-dispatch (fn_g/_tf/h/fp args): player_light_apply,
  camera_state_cache, fighter_trail_emit, ev_stream_dispatch,
  script_op_dispatch, fighter_attach_run, op_cb{1,2,4}_store_ctx8,
  LAB_80137acc. Wrapping is only safe if the variable can never resolve to
  the fn's own guest addr — needs per-site proof, not blanket unwrap.
- 2 literal OTHER-addr delegates (provably safe): LAB_80137cd0 and
  op_wrap_acb08 -> br2_call_generated(0x801acb08). Marginal value (LAB_/op_
  helpers likely reached via clean C anyway).
CONCLUSION: the named-function well is ~dry. 73 renames.map names lack
wrappers but ~85% are intentional delegates; true remaining named targets
are single digits. The real coverage frontier is the ~2679 unnamed FUN_
bodies in generated/SLES_017.22_full_*.c — needs subsystem-partitioned
port agents, not generator tweaks.
PROFILE: disp_ring (always-on per-dispatch ring, fntrace.c) works on live
instances — port 4664 attract demo had 3.24G dispatches. Hot targets are
dominated by CD/kernel loop (0x8019A460 cd_timer_read, 0x80198CEC,
0x8019A5D8 vsync_wait_n) + low-mem kernel dispatches — attract spends most
time in CD/VSYNC polling, so fight-path FUN_ bodies need versus/fight
instances to profile. dispatch_tail/s_dispatch_seq only records BIOS-side
dispatches (game path doesn't call debug_server_trace_dispatch) — use
disp_ring, not dispatch_tail, for game coverage.

=== N+104 — attract demo fight wedges at gs=6 ph=2 (clean repro of fight-init blocker) ===
Fresh build (HEAD 108dbe8, post typo-fixes) boots past CD stall into ATTRACT
FIGHT DEMO (gs=6) but the demo fight WEDGES at ph=2 (scratchpad 0x1f800001)
~frame 1866 and never advances — frames keep ticking ~75fps, VBlank IRQs
deliver normally (event_ring: ISTAT_RAISE bit0 -> IRQ_DELIVER 0x801B0E38
every frame), CD goes IDLE at frame 1866 (last_sector lba 7349, pending
Pause 0x09 phase1, i_stat=0). Game main loop is ALIVE in a vblank-driven
wait — this is a GAME-LOGIC stall in fight init, NOT an IRQ/CD/hardware
stall. ph=2 static >20s, fighter anim ptrs frozen.
SIGNIFICANCE: the attract demo auto-plays a fight through the SAME
battle/fight-init path versus->fight uses — ph=2 wedge is a CLEAN REPRO
of the versus->fight blocker that does NOT need versus navigation or pad
input. Debug here, not through nav_to_fight.
pc at wedge ~0x801B0E38 (generated br2eu_func_801B0E60 — vblank/main-loop
region). Suspect: fight-init state machine waits on a flag/timer/script
event that a clean port or typo left unset — same bug class as N+36/N+37
address typos. ph=2 handler lives in battle_flow_state_machine
(0x8016FCC4, game/menu.c) / battle_intro_fsm path.
NOTE: all pre-existing emus were on STALE binaries (built 17:01; emus
started 15:32-16:53 pre-typo-fixes). Repro requires fresh build.

=== N+38 — pad/SIO channel-driver starvation (versus->fight gate) ===
Navigation agent drove boot->title->attract on fresh instances
(4694-4697): title can NEVER see START because the pad producer is dead.
Pad records 0x801BEFA8/0x801BECA stay 0xFF+zeros boot->title->attract;
decoded pad state 0x1f800068-0x80 never reflects input; sio_trace shows
ZERO 0x42 pad-poll commands (only memcard 0x81/0x52/0x21/0x22/0x23);
pad_buttons=0xFFFF frozen. The records live in the async-I/O channel
driver: SIO fn-table @0x801BE780 (handlers 0x801A38B8/3850/39BC,
generated-only), channel block @0x801FF080 (+0x30=0x801BEFA8), SIO regs
0x1F801040/070. Same starvation class as the CD wedges: deferred-
coroutine/IRQ service starved — clean C spins lack per-basic-block
psx_check_interrupts_at + resumable guest PC, so the channel-driver
service (IRQ- or frame-driven) never runs the pad job. versus->fight
( gs=8 ph=6 ) unreachable without pad input -> SPU-fix verification
blocked. Subagent 4473006d tracing the servicing mechanism + starvation
point. NOTE: uncommitted game/menu.c decomp_trace_record probes +
clean_table.c regen are sibling-agent debug instrumentation, NOT to be
committed.

=== N+105 — frame-1880 DISPATCH FATAL root-caused: scriptop_ret off-by-one ===
Attract fight executes past ph=2 (post-120d72c) then crashes ~frame 1880:
DISPATCH FATAL misaligned 0x00450802, $ra=0x801265F4. Host stack:
script_op_dispatch -> br2_call_generated -> psx_unknown_dispatch -> fatal.
script_op_dispatch read opcode 204 (0xCC) from fighter script stream.
ROOT CAUSE (verified vs generated br2eu_func_80183558, full_38.c:20886+):
clean scriptop_ret (game/vm.c, guest 0x80183558) tested `depth-1==0` for
the return-stack epilogue — i.e. ended at RETD==1 and never popped slot 0.
At RETD==0 it stored RETD=-1 (0xFFFF) and read RETSTK[0xffff] -> VX_PC =
garbage (observed -> opcode stream desync -> 0xCC -> misaligned jalr).
Generated block_80183594: `lh RETD; RETD==0 -> epilogue 0x801835C4;
else RETD-- ; PC=RETSTK[(RETD-1)&0xffff]`. Fix (in tree, vm.c): test
RETD==0 first, then d=RETD-1 index — matches generated exactly.
This was a LATENT clean-port bug unreachable until ph=2 completed.
Lesson: the coarse "uninitialized/edge-case in ported state machines"
class — verify boundary (==0 vs ==1) against generated delay-slot/branch
semantics, not intuition.

=== N+106 — deterministic pc=0 crash isolated to TCB/RestoreState path ===
Post-scriptop_ret-fix, attract fight crashes at IDENTICAL psx_cycle_count
(~1289745198-201, ±3 cycles) every run: cpu.pc=0, epc=0x8019A600 (inside
vsync_wait_n guest 0x8019A5D8 — VBlank coroutine boundary, correctly
self-thunked), ra=0x801265F4 (main-loop ret after jal battle_intro_flow
0x80167330), current_func=0x000029CC (BIOS RestoreState), last_store_pc
0x80192EBC (cd_cmd_queue_load). wedge_kind=fatal.
Mechanism (traps.c forensics): func_000029CC RestoreState reaches fatal via
`jr $k0` where k0 = *(TCB+136) = saved-PC slot; TCB = *(*(0x108)) chain.
pc=0 means the saved context's PC slot was 0/corrupt at restore — SAME
class as N+101/N+102 callee-saved/unwind-context corruption. Deterministic
cycle count = perfect repro for bisecting which save/restore path zeroes it.
Suspect chain: VBlank IRQ fires inside vsync_wait_n's coroutine wait ->
RestoreState resumes a TCB whose saved PC slot was never written (thread
created but not yet Run, or slot offset wrong) -> jr to 0.

=== N+107 — x28 stale-restore repro found post-round (battle_intro_flow 0xf0) ===
After c3b444e (scriptop_ret) the attract fight now COMPLETES A ROUND and
reaches battle_intro_flow state 0xf0 (post-round banner), then dies at
game/menu.c:1969 `DAT_1f800010 = 0` — host EXC_BAD_ACCESS at
0x801fff7800000001 (lldb, ports 4755/4757/4759).
Register forensics: faulting insn `ldr x8,[x28]; strb wzr,[x8]` — x28 should
be &g_DAT_1f800010 (0x11127f8d8) but holds 0x111270000 = g_irqctx_ring .bss
base (ring written by interrupts.c:1762). x25 same stale value; x19/x20/x22/
x23/x27 correct. lldb watchpoint on g_DAT_1f800010 fired ONLY at bind —
global NOT corrupted; x28 itself restored STALE across setjmp/longjmp or
fiber/IRQ context switch. SAME class as N+101/N+102 + dirty_ram_interp.c:2788
noinline fix + crash_trace.c:134 note.
NOTE: may share root cause with N+106 pc=0/RestoreState crash — both are
context-unwind corruption (saved-PC slot AND callee-saved x19-x28). Prior-art
stopgap exists: menu.c:2794 frame_divisor_get bypasses a cached bound ptr for
the identical symptom; faithful fix = runtime register preservation across
the interrupt boundary (interrupts.c/psx_fiber.c/traps.c).

=== N+108 — full clean-tree audit sweep launched (every game/*.c covered) ===
Following the proven audit pattern (N+103 port waves; 1619f14 caught 4 real
bugs in ani.c/gfx.c — recip-table base off 0x28, packet-mask swap,
gte_dpct_vec3 a3-vs-a2 deref, 6-vs-12 compare loop), a complete audit of
every clean source file vs generated/SLES_017.22_full_*.c was launched in
parallel, partitioned by disjoint file sets:

- menu.c (16K lines): CLEAN BILL — heap_serialize tag cluster + state
  machines verified instruction-level (agent 35a3baac).
- ani.c + gfx.c: DONE — 4 proven bugs fixed in 1619f14.
- battle.c + fight.c + action.c: agent d7e74954 (running).
- vm.c + ev.c: DONE — 8 proven bugs fixed in 545b0d5 (scriptop_cmp
  src decode, scriptop_bsr sub-dispatch rewrite, bra_random_pick
  off-by-one, bra_target sel==0 fallbacks, scriptop_gpad mask+shift,
  ev_cmd_dispatch arg swap).
- save.c (9.5K): CLEAN BILL — ~19 functions verified incl. memcard_tick,
  memcard_slot_poll, save_slot_state_fixup block-level (7d0fef87).
  Flagged: weak `return 0` stubs memcard_block_verify /
  job_result_poll_stable remain a live stub risk (follow-up needed).
- resolve.c + bind.c + game.c: DONE — 5 proven bugs fixed in 9ce6b9a
  (task_table_init width, cd_channel_init_seq spurious calls,
  memcard_info_query return, scratchpad u16 widths,
  cd_seek_state_machine stub->B0:0B trampoline).
- cd.c (5.5K): DONE — 10 proven address-typo bugs fixed in 156e2a1
  (flag/pointer/state cell nibble typos; agent 00a13abf).

Bug-class checklist per audit: hex/address typos, host->guest pointer
domain leaks (guest_at vs br2_gaddr), MMIO via raw guest_at vs br2_ld*/st*,
stack-depth off-by-one, G16-vs-GU16 signedness, loop-bound mismatch,
wrong arg register. Delegated `br2_call_generated` thunks are correct by
construction and skipped. Named-unwrapped census: 75 functions remain —
64 self-delegates + 9 indirect-delegates + 2 allowlisted; all correctly
unwrapped (would infinitely recurse through clean dispatch otherwise).
FUN_ marathon (~2668 unnamed) deferred until the x28 gate falls and live
combat is verified.

POST-AUDIT RUNTIME CHECK (build @9ce6b9a+docs): fresh headless run still
hits the SAME context-unwind gate, new manifestation — frame 1899,
FAIL-FAST `unknown dispatch addr=0x03FF0100 ra=0x801265F4`, cpu.pc=0,
epc=0x8019A648 (VSync region, same ra as N+106). Confirms the 23 audit
fixes did not cause it and did not clear it — x28/RestoreState root cause
(445d995c) remains THE gate for attract-fight completion and versus.

=== N+109 — x28 root cause CONVERGED by two independent analysts ===
Both read-only analysts independently converged on the same mechanism:
`exception_jmpbuf` (interrupts.c:748) is a SINGLE GLOBAL jmp_buf shared
across all host fibers, armed per-IRQ-delivery inside
psx_check_interrupts's setjmp loop (~L2063). g_exception_owner_fiber
tracks only the last-armed frame. A longjmp fired from
interrupts.c:~1027/~1214 or traps.c:~669 against a jmp_buf captured by an
ALREADY-RETURNED psx_check_interrupts activation resurrects that dead
frame: stale SP/PC + stale x19-x28 — including x25/x28 pinned to
&g_irqctx_ring (.bss base 0x111270000) by the ring writes at
interrupts.c:1762/1881/2235 which the -O2 compiler keeps in callee-saved
regs across the setjmp. Explains BOTH symptoms: stale x28 in
battle_intro_flow AND poisoned TCB save (resume_pc=0 -> BIOS
RestoreState 0x29CC jr $k0 -> pc=0 / 0x03FF0100 unknown-dispatch).
In-tree precedent: dirty_ram_interp.c:2787 noinline fix for the same
-O2-caches-static-base-across-setjmp hazard; menu.c stopgaps at
frame_divisor_get (2793) and battle_intro_flow 0xf0 (1969).
Nested-delivery clobber: interrupts.c:2051's own comment documents the
inner-setjmp-overwrites-global hazard; the memcpy save/restore guard
only protects the normal-return path.
Faithful fix direction (per CLAUDE.md -1): fiber-scoped exception
escape or epoch-validated jmp_buf — never a per-site workaround.
Recommended instrumentation: epoch tag on exception_jmpbuf + assert
jmp_buf's captured SP/PC inside a live psx_check_interrupts frame
before honoring deferred longjmps (traps.c:659, interrupts.c:1214).

=== N+110 — audit campaign RESULTS: 30+ proven bugs fixed ===
Committed: 1619f14 (ani/gfx x4), 545b0d5 (vm/ev x8), 156e2a1 (cd x10),
9ce6b9a (game/bind x5), 6a27033+552b9d9 (action.c apply()/ani_fixup —
~20 motion-offset/deref/loop fixes vs generated 0x8017A05C/0x80181DB8),
4aa1756 (save.c stub removal + br2_guest_frame staging + cd.c
cd_file_open_seq rewrite + gen_clean_table ARG_GPRS for
cd_dir_entry_store v0/v1 arg domain).
Clean bills: menu.c (heap serializers+SMs), save.c (~19 fns block-level).
Fixed by parent: menu.c memcard_block_verify — host .bss buf made
guest_of()=0 so job_wait_pump DMAs card data to guest addr 0; now
br2_guest_frame(0x80) per generated stack-buffer semantics.
Combat audit flagged NOT-fully-audited: fighter_script_fsm (12.7K gen),
fighter_contact_resolve (3.2K), fighter_pair_update (4.6K),
camera_aim_offset (4.3K), battle_camera_step, entry_setup, part_link —
next audit wave targets.

=== N+111 — x28 fix WORKS: frame 1899 -> 2195, new blocker surfaced ===
ba9ede73 implemented the converged fix: s_exc_jmpbuf_live liveness token
paired with exception_jmpbuf (interrupts.c), armed at setjmp, cleared on
frame exit, saved/restored in lockstep with the nested-delivery jmp_buf
memcpy; all longjmp consumers refuse dead frames (deferred_exception_
longjmp, psx_rfe_escape_check, traps.c pending-honor). Faithful: stale
RFE escapes now decline (g_stale_exc_longjmp) instead of resurrecting
dead frames — cpu->pc/GPR commit already happened so dispatch continues.
VERIFIED: fresh headless run passes frame 1899 (was deterministic crash)
and reaches frame 2195 — attract fight runs ~300 frames deeper.
NEW BLOCKER at frame 2195: FAIL-FAST unknown dispatch
addr=0x8019AA94 ra=0x8019AA9C a1=0x1F801070 (MMIO), pc=0xB0 BIOS-svc
context, epc=0x8018AA60. 0x8019AA94 sits in cd_chan_slot_set
(0x8019AABC) region — CD-channel path, possibly same unwind class or a
new port bug. Under investigation by ba9ede73.

=== N+111a — frame-2195 blocker analysis (0x8019AA94 mid-block dispatch) ===
0x8019AA94 is NOT a compiled block leader — dispatch table has
0x8019AA90 (lui) and 0x8019AA9C (epilogue) bracketing it; AA94 is the
`jal` insn inside br2eu_func_8019A8EC (CD-channel region, ra would be
0x8019AA9C). No reference to AA94 exists anywhere in generated/,
dispatch, recompiler seeds — nothing legitimately targets it statically.
psx_slice_block is PARKED (g_psx_precise_slice off) so blocks run
atomically — resume can't come from cycle slicing. Crash context:
pc=0xB0 (BIOS svc), a1=0x1F801070 (MMIO), epc=0x8018AA60. Most likely:
a guest indirect branch (jalr/jr/bcond) or exception/RFE resume landed
on AA94 — i.e., saved-PC corruption of the SAME unwind family, OR a
recompiler discovery gap for an indirect target the analysis missed.
Distinguish: if cpu->pc was SET to AA94 by runtime resume logic vs a
genuine guest jalr — check dispatch_tail/xprobe in the run report.

=== N+112 — unwind theories EXONERATED; crash is guest-data corruption ===
ba9ede73's liveness fix committed (3fd28d2) but crash persists
nondeterministically (frame 1899 OR 2195 across runs). freeze_check:
stale_exc_longjmp=0, esc_dec_deadframe=0 — dead-frame guard NEVER fired;
22K legit RFE escapes all healthy; thread_ctx_ring EMPTY (HLE scheduler,
all longjmps same-fiber). Cross-fiber jmp_buf theory DEAD.
NEW LEADING THEORY: 0x03FF0100 = low32 of a HOST pointer (host addrs
~0x1xxxxxxxx) stored in guest RAM then used as jump target — AGENTS.md
§5 host-pointer-where-guest-addr class. addr==a0 pattern = indirect
call through corrupted fn-ptr/struct field. Nondeterminism fits a
timing-dependent data race / heap overflow (N+102 heapB→heapA .ANI
overwrite suspect). Agent b8f38c30 tracing the leaking write via
guest_watch on the corrupted pointer's guest address.

=== N+113 — crash = guest-memory ALIASING, not host-pointer leak ===
b8f38c30 traced end-to-end: task_pools_tick (battle.c:8155) walks task
node @0x801D0C20 whose node[2]/node[4]=0x03FF0100 (GTE data-reg contents)
→ task_invoke→call_guest(0x03FF0100)→unknown dispatch. addr==a0==
0x03FF0100 explains all crash fields.
The write is FAITHFUL generated swc2 GTE stores in func_801A5A0C
(full_50.c, PCs 0x801A5B40..80, frame 1898), s0=0x801D0C10, storing
cpu->gte_data[12..14] over the live task node. Proven via PSX_GWATCH.
Caller func_801A8BC8 iterates slot table *(0x801BEF24)=0x801E6260.
Pool links to 0x801D0C20 at guest 0x1FF418/0x1FF420.
Host-pointer-leak theory DEAD (bg_count=0; value deterministic, not ASLR).
REAL BUG = memory aliasing: context-init writes into pool-owned memory.
Either slot-table entry stale OR pool alloc/free double-booked block.
Agent 710ceb0d determining which side diverges.

=== N+114 — frame-1899 crash FIXED: host-pointer truncation in prim_group_build ===
b8af3cef found the divergent write: game/battle.c prim_group_build passed
(int)(uintptr_t)puVar3 (HOST ptr into batch-descriptor table) to
prim_batch_emit, which indexes it via guest_at(param_1+off) as a GUEST
address. Truncated host ptr corrupted task-pool node @0x801D0C20 with
GTE data -> wild call_guest(0x03FF0100). Fix: br2_gaddr(puVar3).
Committed ee5c6d1. VERIFIED: frame 1899 -> 2195, deterministic under
BOTH schedulers (HLE TCB + fiber, PSX_HLE_SCHEDULER=0).
NEW BLOCKER frame 2195: unknown dispatch 0x8019AA94 — mid-block PC =
the `jal 0x8019AD60` insn inside cd_intc_dispatch (renames.map:93 ->
0x8019a8ec, delegated game/cd.c:5433). ra=0x8019AA9C = correct post-jal.
in_exception=1, current_func=0x00002970 (OpenBIOS kernel restore),
epc=0x8018AB6C, cpu->pc=0xB0 (kernel gate). Value exists NOWHERE in
2MB RAM+scratchpad -> COMPUTED not loaded. Leading theory: IRQ-resume-PC
-> COP0.EPC -> guest ExceptionHandler saves mid-block EPC into TCB
reg[R_EPC] slot; kernel RFE `jr k0`(k0=TCB[136]) resumes at it.
Dirty-RAM interp should handle non-leader PCs via clean_game_text_miss
(dirty_ram_interp.c:2836) — fail implies different dispatch path fired.
NEXT: instrument writer of cpu->pc=0x8019AA94 (psx_restore_context_from_tcb
traps.c:378 / func_000029CC jr k0 / g_exception_real_epc interrupts.c:2492 /
br2_irq_poll clean_bridge.c:319). ALSO audit other (int)(uintptr_t)
host-ptr truncations in battle.c siblings (prim_packet_init_tex, hud_*,
sprite_rect_*) — same bug class.

=== N+115 — frame-2195 FIXED (MOVZ/MOVN) + host-ptr audit purge ===
d963244: dirty-RAM interp SPECIAL decoder aborted on funct 0x0B (MOVN,
MIPS32-only) in the delay slot at 0x8019AA98 after BIOS RestoreState
resumed at the mid-block jal EPC 0x8019AA94. R3000A ignores undefined
SPECIAL encodings (psx_interpreter.c:294 documents this); dirty decoder
was stricter than HW. Now ignored. Permanent dirty_reject_log debug cmd
added (ring buffer -> freeze_check/debug server) — pinpointed instantly.
VERIFIED: frame 2195 -> 37,887+ still advancing, 0 rejects.
5c0c7d4: audit agent f7a4b5b7 found+fixed ~17 more host-ptr-into-guest-
addr bugs (battle.c sprite_rect/hud/prim_packet_init_tex; menu.c
match_setup_apply/fighter_*/battle_hud_draw/status_text_display incl.
restored dropped a1 arg; inverse-direction battle_camera_step/
fighter_trail_emit). All vs generated oracle. Build green.
SUSPECT-ONLY remaining: battle.c:594/633 indirect fn_lookup host-vs-guest
ambiguity; task-node mixed host/guest field access — need runtime proof.
NEXT GATE: attract full-round completion + versus->fight nav verify.

=== N+116 — runtime stable to ~frame 50K; TWO layered defects found ===
Verification agent 604fcee7: ~270fps sustained to frame ~50-55K then
deterministic freeze/crash. Attract completes ONE sequence (gs=3->5->6
->5 by ~frame 4000) then parks at gs=5 ph=2 for 45K+ frames — attract
retrigger dead. Versus nav BLOCKED at mode menu: input injection works
at SIO layer but tx_writes frozen at 15556 — ZERO SIO transactions.
DEFECT 1 (primary): kernel thread starvation after gs=6->5 (~frame
2195). TestEvent(a0=0xF1000008,a2=0x01000201) waiter at ra=0x8019D550
spins forever (1.46M TestEvent vs 96 DeliverEvent); sp_ring frozen at
27303 — no context switches after ~2196; CPU sits in dirty-RAM interp
(func=0x000027EC mode=INTERP) during every VBlank. Pad-driver/SIO-poll
thread asleep -> dead input, dead attract.
DEFECT 2 (secondary): terminal crash ~frame 38-55K nondeterministic:
unknown dispatch addr=0x000027EC ra=0x000027EC a0=0x07DD6C94 — low-RAM
BIOS/kernel code reachable only via dirty-RAM interp; a0 is a HOST
pointer leaked as guest arg (same class as 5c0c7d4 purge; missed site).
Port agent 53328f16 committed b34253b: 12 hot fn_stats functions ported
(cd.c+3, gfx.c+9), clean_table 1220 wrappers, no regression.
NEXT: agent on TestEvent/DeliverEvent starvation (critical); agent on
0x27EC host-ptr leak.

=== N+117 — ALL symptoms = ONE root cause: host-ptr leak -> kernel RAM spray ===
Two agents converged independently:
- "TestEvent starvation" (0bc00341) is NOT a scheduler bug: CPU dies
  because kernel RAM is overwritten — BIOS exceptionHandler code @0x27EC,
  EvCB table @0x8E08, kernel heap all sprayed with GPU/GTE primitive
  packet words. TestEvent count FROZEN (CPU stopped reaching BIOS calls);
  pc=0 livelock in dirty-RAM interp on corrupted code.
- Wild writer = generated br2eu_func_8018A9C8/8018A9E8 (GTE prim-emit /
  heap-serialize tag-0x40051 mode-7 deserializer, emitted by
  game/menu.c:14484) running with a0/a1/a3 = TRUNCATED HOST POINTERS
  (0x60208E54 = low32 of macOS heap 0x0000000260xxxxxxxx). Computes
  output cursor t3 into kernel RAM; first fatal write frame 3159
  (0x011C0129 -> phys 0x27EC), wild writes begin frame 2198 = exact
  starvation onset.
- Call chain: call_guest <- prim_task_run (battle.c:13240) <-
  channel_script_emit_triple (battle.c:16863) <- clean_try_dispatch.
- Host ptrs already inside guest-visible task/heap structures consumed
  by the deserializer — producing store predates frame 2198, propagates
  via TCB save/restore. Same bug class as ee5c6d1/5c0c7d4.
NEXT: find the clean-C store writing host ptrs into heap/task structs
feeding func_8018A9E8 (menu.c:14484 serializer emitter; battle.c
prim_task_run/task node fields [2]/[3]/[4]; node_list_collect
battle.c:17334 indirect fn(arg) path). Fixing it should resolve
starvation + dead input + attract park + 0x27EC crash simultaneously.

=== N+118 — ROOT CAUSE FIXED: register-file poisoning via dropped call args ===
0a986712 found THE bug (822088f): prim_task_run (battle.c, guest
0x8018A4F8) invoked each 16-byte record's fn ptr via call_guest(fn,
arg, 0) — generated sets a0=rec[2], a1=param_2+lh(task+0xE), a2=param_3,
a3=rec[1] before EVERY jalr. Dropping a2/a3 left STALE register-file
values = truncated host pointers; serializer 0x8018A9E8 used them as
guest cursors -> kernel RAM spray from frame 2198. ONE bug explained:
TestEvent starvation, dead SIO/input, parked attract, 0x27EC livelock.
Fixes: call_guest4 (full a0-a3 marshal, clean_bridge.c); prim_task_run
exact ABI; ring_effect_task_update missing guest_at() (host segfault);
terrain_packet_gen non-faithful port (inlined phantom loops, param_6=-4
-> 4G-iter livelock) -> delegated per section 4.
VERIFIED: frame 123K+ advancing, kernel RAM intact, all poison counters
clean. GAME SHOULD NOW BE PLAYABLE — attract retrigger + versus input
alive. NEXT: full attract round + versus->fight verification.

=== N+119 — game runs INDEFINITELY (frame 369K+); CD doorbell = THE blocker ===
cc9eaa8e verified 822088f: frame ~369,480 sustained ~350-430fps, ZERO
crash; kernel RAM intact, all poison counters clean, context switches
resume every frame. Poisoning fix CONFIRMED end-to-end.
NEW PARK STATE: attract cycles gs 3->4->5->6->5 then freezes at
gs=5 ph=33 (0x21) from ~frame 4068 — real park (anim structs static).
Versus UNTESTED (nav needs ph=1 which never recurs while parked).
SINGLE REMAINING BLOCKER = known AGENTS.md section 8 CD deferred-command
stall: cdrom_command_history frozen at frame ~3772 — last cmd=0x09
(seek) pending active=0 phase=1 stat=0x02; ZERO CD commands for 124K+
frames. Game thread sleeps in TestEvent (B0:11, frozen) waiting for CD
event that never arrives; only live BIOS traffic = B0:0x17 x1/frame
from ra=0x8019AA9C inside CD-channel fn 0x8019A8EC. Boot loads real
Mode-2/XA fine (last_sector lba=6413 @ frame 3767) then NEXT file's
command never reaches device — deferred command-issue / register-window
doorbell in cd_seek_param_issue->cd_reg_window_op->worker.
EVERYTHING downstream (attract fight data, versus, input) gated here.

=== N+120 — CD stall = lost second Pause completion (IRQ accounting race) ===
c087ee4e disproved the doorbell theory: CD device healthy/idle; last cmd
seq478 Pause(0x09) @frame 3772 executed+acked device-side. Pattern: each
file = Setloc+ReadN streamed, ended by IRQ-context Pause, then next
file's Setloc 2-5 frames later. After seq477's IRQ-Pause @3767 the
loader issued ANOTHER Pause (seq478, the per-file "ensure stopped"
prologue cd_cmd_issue_sync(0x09)) @3772 — TWO Pause completions in close
succession. Only ONE completion IRQ appears consumed; the main-context
waiter for seq478 parked in TestEvent(0xF1000008) forever -> file-load
SM never advances to Setloc. cd_seek_state_machine (cd.c:3531) spins
1.68M TestEvents; no CD worker/seek/job fn ever runs again.
HYPOTHESIS: completion-accounting race in runtime CD IRQ presentation —
second CDIRQ_COMPLETE masked because irq_flag still set during first
processing, or response channel drains only one queued response per IRQ.
NEXT: verify double-completion IRQ accounting in runtime CD model
(set_irq/present_cdrom_irq re-arm after ack); faithful fix = second
completion must re-present, not game-side fake.

=== N+121 — CD IRQ accounting EXONERATED; stall = guest deferred-job starvation ===
3f558ecc disproved N+120 hypothesis with hard trace: BOTH Pause
completions produced set_irq+fire_irq and both responses consumed
correctly (cdrom.c:939-1040 level re-arm works). seq478's COMPLETE fired
inside the ACK ISR (pause delay 5000cyc when already stopped,
cdrom.c:2102) — guest consumed via pump's internal loop. irq_flag=0,
FIFO empty, device genuinely idle.
REAL STALL: guest-side deferred-job starvation. DeliverEvent
(0xF0000009) the SM waits on comes ONLY from cd_dma_chan_reset
(cd.c:4957) via IRQ3/CD-DMA completion — which never ran since before
seq477. No CD cmd/DMA/reg_window_op since frame 3772. Deferred
coroutine alive (cd_intc_dispatch ~1/frame no-op, empty INTC mask) but
NO QUEUED WORK. The response type-2 completion handler for seq478
should post the job/callback leading to chan_reset/next Setloc — it
didn't, OR that job is scheduled only on CD IRQ2 edges which stopped
when drive idled.
NEXT: instrument whether type-2 response handler posts the job, and
whether the reg-window command worker is IRQ2-edge-scheduled only.

=== N+122 — CD stall localized: staged cmd never flushed to 0x1F8018xx ===
cf62c7bb: NOT a livelock-hang — frame counter advances; guest SM pinned
mid-iteration (s3=1). Chain: state_machine->cd_cmd_queue_load->
cd_cmd_issue->cd_result_wait_a->cd_timer_read->vsync poll. Command IS
staged each frame (cd_cmd_queue_load writes queue regs -> 0x1F8010E0
DMA channel shows start bits cleared = ran/completed) but ZERO
0x1F8018xx writes after frame 3772 — flush via cd_reg_window_op/
cd_seek_param_issue/cd_file_read_seq NEVER invoked (pc_probe=0 hits).
SM waits on DeliverEvent(0xF0000009) that only comes via CD-DMA ch4
completion -> cd_dma_chan_reset — never armed.
PRIME SUSPECT: clean-C busy-waits in cd_queue_push (cd.c:5380) /
cd_queue_drain (cd.c:5323): `while((br2_ld32(DB32G(0x801b8428)) &
0x04000000)==0) br2_irq_poll();` — EXACT section-4 pattern starving the
deferred CD coroutine (no guest resume PC) during the IRQ2-edge/DMA-done
window. Faithful fix = delegate those bodies to br2_call_generated.
SECONDARY: DMA done-interrupt possibly masked (DICR bit31=0, cd_intc_
dispatch sees I_STAT=0 -> no handlers).

=== N+123 — queue delegation done (7c3b5c6) but stall persists; ch5 arm missing ===
eeda99c4 delegated cd_queue_push/drain per section 4 (clean busy-waits
starved coroutine). Verified live but INSUFFICIENT: real missing link =
whatever arms the channel-5 DMA transfer whose completion delivers
DeliverEvent(0xF0000009). cb7_run slot tbl[5]=0x8019B7E0 fires only on
DMA-ch5 (PSX PIO 0x1F8010D0) done; dma_state shows ch5 NEVER started.
dma_trace: only per-frame ch2(GPU)+ch6(OTC 2080w driven by
cd_cmd_queue_load) active. DMA IRQ path audited clean (complete_transfer
-> mask -> psx_irq_raise(3) on 0->1 edge; DICR=0x00900000 = genuine
ch4-only mask, not a bug).
The arming is upstream, gated behind the same event -> deferred
coroutine/job-pump path (cd_chan_full_init ctx @0x801BCD50, resume via
B0/0x19; job slots @0x801BE768=-1 -> chan_job_pump no-op).
NEXT: find who writes tbl[5] (@0x801BDE08) / owns ch5 object, and why
the deferred job that arms it never fires. ALSO: remove env-gated
fprintf diagnostics in cd.c (PSX_CB7) before final — no-fprintf rule.

=== N+124 — ch4 (not ch5) is the CD-DMA channel; circular deadlock narrowed ===
5ae4044c: ch5 irrelevant — ch4 (0x1F8010C0) is CD-DMA. tbl[4]=0x8019B7E0
(cd_dma_chan_reset) sole DeliverEvent(0xF0000009) sender on ch4 done.
Channel-xfer submit = func_0x8019B954 (programs 0x1F8010C0/C4/C8
CHCR=0x01000201; last arm frame 3704). Driven by descriptor block
0x801BDEA4-0x801BDF08; request-state cell 0x801BDF00=0 (no pending).
DEADLOCK SHAPE: SM waits event 0x20 <- needs ch4 done <- needs 0x8019B954
submit <- needs new request posted @0x801BDF00-08 <- poster upstream in
same CD/sound pipeline gated behind the event. All game threads ASLEEP
(only B0:0x17 RFE ~4-5/frame); coroutine alive but no work. Response-
callback slots 0x801BC848/4c=0 likely legit (no read session).
NEXT: wtrace writes to 0x801BDF00/04/08 before frame 3772 on fresh run
-> identify producer fn + its gate; compare vs Beetle oracle at ~3772.

=== N+125 — REFRAME: game spins render loop, logic frozen at ph=0x21 ===
91adcac3: NOT kernel deadlock — per-frame render loop alive (ch2 GPU +
ch6 OTC DMA every frame) while ALL game-logic state frozen. Scratch:
gs=5, DAT_1f800001=0x21(33), 0x801f80008e=199. battle_intro_flow
(menu.c:1846) + match_loop (game.c:1434) both fall through on unhandled
phase 0x21 -> frame_tick forever. cd_reg_window_op descriptor writes
(0x801BDF00/04/08) all inside generated 0x8019B954 tail of cd_reg_
window_op, called 3x/stream-chunk by func_0x801565EC (sound/stream
driver CD->SPU poster, NOT file loader); last activity frame 3704.
chan_reset ran frame 3705 delivering the event ONCE; no DeliverEvent
since. decomp_trace: last loads file_id 0x2D/0x28/0x2C then silence.
UNRESOLVED: who wrote 0x21 to DAT_1f800001 (no clean-C site; ~261
candidate write_byte(+1) sites in generated, vm.c/demo territory);
whether Pause@3772 completion should yield a 0xF0000003 DeliverEvent
the script VM consumes (cls3 CD events from 0x80195B4C/B74/B9C).
NEXT: gwatch 0x1F800001 early (~frame 3500) for the 0x21 writer; probe
cls3 CD done->event chain in runtime cdrom.c/interrupts.c.

=== N+126 — ph=0x21 freeze FIXED (70760d5); whole class = stale x28 restore ===
ead766a5: DAT_1f800001=0xff compiled to strb w28 — constant hoisted into
callee-saved x28, restored STALE by IRQ setjmp/longjmp (N+107/N+109
class). gwatch proved f1->0x21 write at frame 3805 via strb w28.
Fixed via br2_st8 (arg-reg materialization) at menu.c:1980 + game.c:1541.
VERIFIED: past freeze to ~frame 7499. NEW SIGSEGV same class inside
dirty_ram_text_native_ok_ranges_from (memory.c:543) — file-static base
cached across setjmp boundary (the noinline hazard dirty_ram_interp.c:
2855 documents). THE REAL FIX = runtime callee-saved register
preservation across exception_jmpbuf — retires ENTIRE class (x28
crashes + strb-w28 + memcmp). N+109 direction, still unfinished.
OTHER DAT_1f800001 const-write sites (menu.c 1002/1148/1399/1736/1828,
game.c 1458/1468/1524/1529) same latent hazard.

=== N+127 — CLASS-LEVEL FIX: callee-saved preservation + ret_zero_d ===
cbe71b39 landed fbc8d52, verified frame ~153,875:
1. interrupts.c psx_check_interrupts wrapper snapshots ALL host
   callee-saved regs (arm64 x19-x30 / x86-64 rbx,rbp,r12-r15) across the
   setjmp/longjmp exception boundary, restores on return; fast-path
   gate when no IRQ pending. RETIRES the entire stale-x28 class
   (N+107/N+109/N+126): strb-w28 phase writes, wild file-static bases,
   memcmp crashes. Composes with s_exc_jmpbuf_live token.
2. save.c ret_zero_d: 3 semantic bugs vs generated 0x8016DADC (sel==t9
   not sel==1; field-src a0+8+t9*8; centre cnt_b>>(t9&31) not cnt_b>>1)
   — ~28,935 bogus records trashed RAM. PLUS br2_ldN/br2_stN conversion:
   cached guest_at() host ptrs overflowed 2MB arena into .bss at
   heap-A 0x801Cxxxx (frame-7869 wild memcmp root).
NEXT: continue runtime verification — does attract now loop and
versus->fight navigate? CD stall may also be resolved (it was gated
behind corrupted state).

=== N+128 — CD streaming UNBLOCKED (451 cmds, real sectors); new stall @8092 ===
8f56592a verified c88581d: frame ~268K sustained ~460fps, no crashes.
CD now streams correctly — 451 commands past old seq478 freeze, real
Mode-2 sectors (lba 4644). Attract progressed gs5->gs6 ph2. versus
untested (game parked before pad read).
NEW DETERMINISTIC STALL frame 8092 (every run): Thread A file-load
waiter TestEvent(0xF1000008) spins v0=0 forever; Thread B cd_intc_
dispatch spins B0:0x17 waiting a CDROM IRQ that already fired+acked.
INT2 Pause-complete WAS delivered+acked (irq_flag->0); game's
completion path never issues next SetLoc/ReadN, never DeliverEvents.
Suspect: game expects a SECOND CD interrupt (INT1 data-ready for tail
sectors) the device model never raises because Pause cleared
CDSTAT_READ/reading — lost/never-generated IRQ at file-tail boundary.
No context switches since ~frame 8090.
NEXT: instrument frame-8092 boundary — what IRQ does game expect after
Pause-INT2; check stop_read_stream() (cdrom.c:2261) suppressing INT1,
or INTC mirror bit never latching for cd_intc_dispatch B0:0x17 wait.

=== N+129 — frame-8092: lost wakeup = DeliverEvent to CLOSED EvCB ===
d28e4419: CD side CLEAN (irq_flag=0, int1_lost=0, queue empty, Pause
delivered INT3+INT2 correctly). NOT a suppressed INT1.
REAL SUSPECT: DeliverEvent(0xF0000003,0x40) fired at freeze instant
(ra=0x80198824 cd_read_finish_cb, rep=43) but EvCB table has NO
0xF0000003 events — slots now hold 0xF4000001/0xF0000011 CALLBACK
events; slot8 = only MARK event (0xF0000009 spec0x20 = ch4 xfer-done
flag the waiter spins on). If the 0xF0000003 EvCBs were already CLOSED
when delivery ran -> matched nothing -> file-load wakeup silently
dropped -> main never proceeds -> never kicks next ch4 xfer -> waiter
spins forever.
NEXT: trace when/why 0xF0000003 EvCBs (specs 0x10/0x20/0x40/0x80/0x8000)
close vs last DeliverEvent — evcb_walk_dump/fntrace on CloseEvent
callers. If closed early due to spurious earlier completion our runtime
delivered, that's the faithful fix site (upstream, not the drop).

=== N+130 — EvCB theory DISPROVEN; blocker = deferred cmd-issue (AGENTS.md §8) ===
681d4dfb: 0xF0000003 EvCBs were KERNEL CD-driver events closed
legitimately at frame 170 (~150f before game events opened @318, ~7900f
before freeze). Game NEVER opens class-0xF0000003 EvCBs (all 21
OpenEvent accounted: kernel CD + kernel 0x8DD8 + game's 9). The
DeliverEvent(0xF0000003,0x40) drop is harmless by design. Real waiter
slot8 (0xF0000009/0x20 ch4-done) open+enabled+receiving fine.
TRUE BLOCKER (unchanged, sharper): at frame 8092 no command is even
QUEUED to device — pending.active=0, cycles_to_next_event=UINT32_MAX,
i_stat=0x3. Main spins TestEvent waiting DMA/seek completion that can't
come because next command never reaches cd_cmd_issue. The deferred
command-issue/doorbell path cd_seek_param_issue->cd_reg_window_op->
worker — same coroutine/resume-PC starvation class that forced the
br2_call_generated delegations.
NEXT: trace why guest CD worker never doorbells cd_cmd_issue post-8092.

=== N+131 — deferred-job chain mapped; probe points identified ===
a3aa61f1 (read-only): cd_response_process = pure drainer (no job post).
File-load completion chain: finish_cb tail -> mc_session(cd_init_done_cb)
-> Pause -> (*DAT_801bc7b0=0x80195b9c cd_bios_cmd40_b) -> DeliverEvent
(0xF0000003) -> wakes loader -> SHOULD submit next ch-job via
cd_chan_job_submit (0x8019ea98, gated @0x801febc0 state + @0x801be768
counter, 4-slot cap @0x801fef60/0x801fef20). Pumps cd_chan_job_pump
(0x8019f80c)/chan_job_pump(0x801a0110) consume. DMA-done alt producer:
cb7 tbl[4]=cd_dma_chan_reset.
0x80125708-5C = CD diagnostic STRINGS (data, dead end).
NEXT PROBES: is cd_chan_job_submit called-and-rejected vs never called?
mem_words @0x801febc0/0x801be768/0x801fef60/0x801fef20 at stall;
fntrace on submit/pumps/0x80195b9c; confirm DeliverEvent(0xF0000003)
fires @8092 + waiter wakes.

=== N+132 — frame-8092 "stall" was an AdES crash; call_guest4 dropped $t0 ===
72863a36: the deterministic freeze was a guest CRASH not a CD gate.
prim_task_run record loop `lw $t0,0xC($s2); jalr $ra,$t0` -> callee sees
$t0==fn; call_guest4 left $t0 stale (GTE data 0x00340216) -> emit entry
0x8018A514 `sw $s6,0x3E0($t0)` -> unaligned AdES -> BIOS halt loop that
LOOKED like CD stall (frame froze). Fix: call_guest4 sets gpr[8]=fn
(jalr semantics), restores after. Verified frame 8094, emit clean.
NOW UNMASKED: genuine CD-loader TestEvent(0xF1000008) wait — the real
DeliverEvent(0xF0000003)->waiter chain is the actual remaining bug.
NOTE: debug server lacks SIGPIPE ignore — repeated connect/disconnect
kills runs (exit 141); worth a robustness fix.

=== N+133 — bail-unwind guard sweep COMPLETE (a639dbc) ===
27d32fdf swept ALL clean-C poll loops: cd.c 15 sites + save.c 8 sites +
menu.c one_shot_init (added by me). Every loop waiting on guest/IRQ-
mediated state now returns early on br2_bail_pending(), matching
generated per-block `if (g_psx_call_bail) return;`. The wedge class is
fully closed — br2_call_generated's stale-$v0 no-op during bail can no
longer trap a clean-C spin. Dedup/bounded loops verified safe.
FOLLOW-UP flagged: cd_queue_a/b_status spins lack br2_irq_poll entirely
(pre-existing fidelity gap — generated runs psx_check_interrupts per
iter); state-machine for(;;)+poll:->frame_tick pattern exists in
battle.c/menu.c/game.c — same hazard if bail fires with them on stack.

=== N+134 — host-ptr + arg-order bugs in camera/light path (f5cfd28) ===
8ab3087f proved: camera_state_cache/player_light_apply passed &DAT_*
HOST ptrs where clean callees take guest-u32 addrs (guest jalr passes
0x1F8002F8/0x801F0228/0x1F80033C). dual_axis_coord_xform had arg-order
bug — generated uses $a2=out-base/$a3=object ($a0/$a1 dead); clean read
object from param_3/out from param_2. Fixed + clean_table regen.
Static-proven vs generated; runtime verify pending (combat path).

=== N+135 — CD stall RESOLVED; new gate = prim_task_run record-list crash @8094 ===
8549675d verified: bail-unwind fix WORKS — CD streams real Mode-2/XA
(seq 9113), old stall gone. NEW deterministic crash @frame ~8094:
Null-PC dispatch in prim_task_run record walk — fn=0x00000001 -> PC=0.
Port verified faithful vs generated; corruption upstream: record-list
base/count resolves wrong OR earlier writer sprayed list region.
SUSPICIOUS: 4 consecutive record-fn targets 0x8018A514/518/51C/520 =
MID-FUNCTION addrs inside func_8018A4F8 (not dispatch leaders) — walk
reading a code-ptr/jump-table region as record fn fields. a0 data
cursors 0x8003xxxx, a1=0x801E6158. Artifact /tmp/exit_trace_run2.json.
NEXT: dump task+0x1C chain + record array @0x8003xxxx @frame ~8090.
ALSO 8ab3087f: stepper-descriptor cb fields stored host fn ptrs ->
committed guest literals (menu.c:1015->0x8015caa8, save.c:710->0x80160598).

=== N+136 — frame-8094 crash FIXED (ee57989); SIO TX stall is next gate ===
8005551a root-caused: 4 mis-transcribed nm constants in heap_serialize
(menu.c) — wrong values were MID-FUNCTION addrs inside prim_task_run
(0x8018a514/664, 0x801894fc, 0x8018a3ac) -> psx_dispatch re-entered fn
treating data cursor as task array -> garbage nested walk -> fn=1 -> PC=0.
Fixed to real block leaders (0x8018a714/9f8, 0x80189500, 0x8018a2ac).
RUNTIME VERIFIED: frame 16143+ past crash, attract progressing.
NEW BLOCKER (a7124d65): SIO TX stall — tx_writes frozen @15556 since
~frame<946; pad_buttons set host-side but NO SIO transmit -> guest never
receives pad -> versus input dead. sio_stat=0x0005,sio_ctrl=0x0000,
sio_rx=0x5A,tx_gated=0. Predates 8094 crash, independent. ALSO attract
reaches gs=6 ph=2 then dies at 8094 (now fixed). NEXT: SIO TX resume.

=== N+137 — SIO TX stall = SYMPTOM not device bug; real wedge = CD channel-event wait ===
7462b18e: sio.c/memcard.c device model FAITHFUL + idle (tx_gated=0,
nothing pending/dropped). Guest stopped issuing SIO cmds because main
thread parked in CD channel-event wait: cd_intc_dispatch B0:0x17 storm
(~941K) + TestEvent(0xF1000008) spins. tx_writes frozen = downstream
symptom. Game ALIVE: frame 35322+, CD still loading (lba advancing).
Pad-poll path starved by the CD/event wait. Real question: is attract
PROGRESSING (gs/ph advancing through demo fights) while CD-streams,
or truly wedged on a missing ch4-DMA-done event? The 0xF1000008 flag =
ch4-xfer-done; if CD loads files but event never delivers -> deferred
ch4-arm or DeliverEvent gap remains (same class as original stall but
much later/deeper). NEXT: trace gs/ph progression + which ch4 transfer
should fire the event.

=== N+138 — game reaches frame ~22020 (attract battle intro); kernel-RAM corruption ===
9f444b6e: NOT a CD wedge — hard halt. Game progressed to frame ~22020
(attract battle intro via battle_intro_flow). CORRUPTION: guest SWL
stores to BIOS kernel RAM 0x1AAC rewrote `jal DeliverEvent` -> garbage
-> verifier fn jumped to 0x115DB0 pattern-fill -> PC=0x2934 FAIL-FAST.
Writer: pc=0x80187774 psx_swl in generated br2eu_func_801876BC (GTE
vertex/transform leaf) — store addr = a1+(gte_data[7]&0x1FFC), a1 was
GARBAGE ~0x1AAA (base-ptr arg bad on entry). Called via tail-jump from
battle_intro_flow (menu.c:1847) through state-dispatch trampoline.
0x801876BC invoked via heap_serialize record path (menu.c:14291 case
0x11, B7C0_EMIT nm=0x801876bc CORRECT) — bad a1 comes from record
replay args, same family as nm/arg-order bugs. NEXT: fntrace_arm
0x801876BC before frame 22000 to capture a0-a3; audit heap_serialize
ctx-pointer relocation guard (menu.c:14258) for missed encoding.

=== N+139 — emit-fn a1=0 kernel-RAM corruption FIXED (battle.c prim_list_gte_invoke) ===
6a6dc3a7: bad a1 was NOT heap_serialize — prim_list_gte_invoke used
task_invoke->call_guest(fn,arg,0) hardcoding a1=0/a2a3 stale. Generated
0x801871BC jalr's full a0-a3 (a0=rec[2],a1=param_2+lh(param_1+0xE),
a2=param_3,a3=rec[1]). call_guest4 fix verified: a1=real OT-base addrs.
NEW crash @frame 11068: DISPATCH FATAL misaligned target 0x0000FFE7 —
CPU in kernel/low-mem code jumped to misaligned addr; 0xFFE7 values in
RAM @0x80049xxx-0x8004Dxxx (OT/heap data). Possibly masked before or
nondeterministic. NEXT: fntrace/pc_probe kernel low-RAM range +
0x801264E8 dispatcher family; determine if 0xFFE7 is corrupted OT
entry / saved-context field / jump-table value.

=== N+140 — 0xFFE7 crash CLEARED; game deep in attract @frame 23000+ ===
My probe of verify emu (port 4920, HEAD dcebda1): frame 22288->23235,
PAST the 0xFFE7 @11068 crash AND past earlier 22020 point. CD seq
421190 streaming Mode-2/XA continuously. gs=5 (title/attract area).
SIO tx_writes STILL 15556 — pad input still dead (versus blocker
remains, was symptom of CD wait; now CD healthy so input path itself
needs re-investigation). Frame rate slow (~0.4fps) — heavy CD streaming
or idle wait; agent 44ef3449 characterizing. CRASH GATE CLEARED.

=== N+141 — attract stalls @frame ~23247 (CD channel-event wedge recurs) ===
4920 long-run: frame 23244->23247 over ~15min (~0.01fps). CD seq
421190->421300 (+110 only) — device nearly idle. bioscall total 2M
(TestEvent-dominated). SAME deferred channel-event wedge class as
N+137: game thread parked waiting ch4-DMA-done/DeliverEvent that isn't
firing for current transfer, even though CD device healthy. The
crash-gates are cleared but the DEFERRED EVENT DELIVERY gap persists
attract-deep. 44ef3449 characterizing; 83b9504a on pad-input (fresh
4930 run @frame 739 progressing). NEXT: the recurring deferred
channel-event producer gap — needs the same depth as original CD stall.

=== N+142 — CORRECTION: "stall" @23247 was renderer-slowness, NOT a wedge ===
44ef3449: 0xFFE7 crash CONFIRMED resolved — clean run to frame 27488,
zero faults. The near-zero frame advance I saw was the HEADLESS OPENGL
RENDERER bottleneck (~0.2-1fps effective, 98.6% GPU share), NOT a game
wedge — frames/cycles/vsync all tick correctly (~677K cycles/frame =
proper PAL budget). Attract reaches gs=5 ph=1 (title). Versus nav
INCONCLUSIVE: at 0.22fps only ~33 guest frames in 150s — title window
didn't elapse enough guest-frames; NOT evidence of blocker. N+141's
"deferred channel-event wedge" was misdiagnosis — CD seq barely
advancing because FRAMES barely advance (renderer-bound), not because
game parked. REAL frontier: versus nav needs longer timeout OR faster
renderer; pad-input agent 83b9504a determining if injection works.

=== N+143 — versus-nav speedup path identified ===
44ef3449: renderer is wall (98.6% GPU, ~0.2-1fps). Options found:
PSX_FAST_FORWARD_SPEED env (2-16x or "max"/"0"=unbounded,
manual_fast_forward_multiplier main.cpp:607) + g_turbo_loads_enabled
(turbo_loads TCP cmd / per-game turbo_loads config) runs vblanks
unpaced w/ present-skip (s_turbo_present_skip). For versus verification:
relaunch emu with fast-forward/turbo to get through title->menu->fight
in reasonable wall-time, then nav_to_fight.py. Pad-input agent
83b9504a determining if injection reaches guest at all.

=== N+144 — pad-input chain mapped; upstream SIO issuer not running ===
My probes: pad_status shows host pad connected, buttons=0xFFFF (idle),
sticks centered — host side fine. pad_state_refresh (menu.c:2750) is
clean-ported, runs per-frame from present path, but only DECODES
DAT_801befa8/DAT_801befca (attr_word_decode) into pad-state struct —
does NOT issue SIO TX. The actual pad-read/SIO-TX issuer is UPSTREAM
(kernel pad driver / BIOS pad service filling those cells) and is NOT
running -> tx_writes frozen. Game in ATTRACT (gs=3, auto-demo) — may
legitimately not poll pads; open Q: does attract poll pads for
START-to-skip? If yes+broken=bug; if no=versus needs attract timeout
(300 frames) or attract is stuck. fntrace pad_state_refresh=0 hits is
EXPECTED (clean-ported, bypasses psx_dispatch). Agent 83b9504a on it.

=== N+145 — CONFIRMED: START injected but tx_writes frozen = real input bug ===
My live probe on 4930 (attract gs=3, frame 29659): press START
(0xFFF7,60f) -> ok:true, pad_buttons now shows 0xFFF7 (host override
active) BUT tx_writes STILL 15556 — NO SIO TX even with button pressed.
Guest NEVER reads pad via SIO regardless of input -> upstream pad-read
issuer (kernel pad driver / BIOS pad service) genuinely not running.
NOT attract-legitimate-idle: a real dead path. Versus BLOCKED until
the SIO TX issuer resumes. Agent 83b9504a tracing which guest fn
should issue pad-read SIO TX and why it stopped.

=== N+146 — SIO TX issuer = kernel func ~0x27EC stopped after byte_seq 15464 ===
sio_pc_trace tail: last SIO IO was pc=0x00003650/0x00003640 func=
0x000027EC writing SIO_CTRL(0x1F80104A)=0x1013 + SIO area — KERNEL/BIOS
pad/SIO driver. byte_seq froze @15464. This kernel driver stopped
issuing pad-read transactions -> tx_writes frozen -> guest never sees
pad. BIOS has PAD driver @0x43D0 + PAD_init/PAD_dr services. The
kernel pad-poll thread/driver exited or blocked. Agent 83b9504a
tracing why the kernel SIO driver stopped (event/flag/thread state).

=== N+147 — END-TO-END CONFIRMED: versus blocked by dead kernel SIO driver ===
PSX_FAST_FORWARD_SPEED=max gives ~102fps (vs 0.2fps renderer-bound) —
versus nav now testable in seconds. nav_to_fight.py 5010: reached
gs=5 ph=1 (TITLE) at frame ~5847, held START repeatedly, ph NEVER
advanced to 2 — game cannot see pad input. CONFIRMS kernel SIO/pad
driver (func ~0x27EC, stopped @byte_seq 15464) is THE versus blocker,
not attract-idle. Fast-forward is the verification path forward.

=== N+148 — transcription audit COMPLETE; constant-table surface clean ===
cdcb9eb5 systematically verified ALL hand-transcribed tables vs
generated+EXE: heap_serialize nm constants (84 values, exact multiset
match), tag mask 0x0F04FFFF, jt tables (battle_state_gate,
stage_fight_announce, post_battle_side_commit, mode_name jt1/jt2),
flag_table_dump, task_invoke ABI, all br2_call_generated delegation
addrs, section markers. ONE real bug fixed: mode_name_announce
PRESS START flag v0=0->v0=4 (delay-slot li). The crash-causing
transcription class is now swept clean — remaining un-audited surface
is data-value switches + mask/compare constants (lower risk).

=== N+149 — pad+card input dead: IntRP verifier starvation (root cause) ===
83b9504a traced the dead pad path end-to-end: at gs=5/ph=1 title, frame
advances + VBLANK events deliver, but SIO tx_writes frozen @15556 AND
memcard traffic frozen (@114) -> the ENTIRE BIOS sio0Handler is dormant.
IntRP chain @0x800045E8 has 4 BIOS nodes; all verifiers run every IRQ
(~11 hits) but only handler 0x1BD0 fires. Hypothesis: IRQVerifier/
mcHandler runs BEFORE sio0Verifier in chain order and auto-acks the
VBLANK IREG bit (s_IRQsAutoAck[VBLANK]) -> sio0Verifier sees bit cleared
-> returns 0 -> sio0Handler (readPad x2 + readPadHighLevel +
firstStageCardAction) never runs. Explains both pad AND card freeze,
and why it worked early (15556 TX) then stopped mid-run (~frame 946):
something reordered the chain or enabled VBLANK auto-ack. NOT a wedged
SIO device, NOT wrong BIOS, NOT attract. Faithful fix target:
interrupts.c IREG/IMASK/auto-ack semantics or recompiler COP0/exception
path — NOT sio.c, NOT a shim.

=== N+150 — pad death REFRAMED: downstream symptom of CD seek/event stall ===
8ee50218 DISPROVED the IntRP-starvation hypothesis: interrupt chain is
healthy (game's own cd_subchan_table_install record @0x801FEB40 fires,
cd_intc_dispatch/cd_cb8_dispatch run every IRQ, vsync_wait_n advances).
REAL stall: game wedged in cd_seek_complete_poll (0x8019d500, delegated)
-> testEvent(0xF1000008) spins 2.18M times always v0=0; cdrom_state
pending={cmd:0x09 Pause} stuck, irq_flag=0x03; cd_seek_trace shows 16
identical seeks to LBA 208829 never reaching drive; commands stopped
~frame 30665 in a 0x09/0x0E/0x02/0x06 file-load retry loop. Mechanism:
Pause(0x09) pending completion blocked (irq_flag!=0) and/or INT2
cancelled by next exec_command before pause_complete_delay elapses ->
completion event never delivered -> game never exits CD poll -> never
reaches frame_tick->pad/SIO polling -> tx_writes frozen @15556, mc_*
frozen @114. This is the AGENTS.md §8 deferred-command-issue /
register-window doorbell blocker surfacing as the "pad bug". Faithful
fix belongs in runtime/src/cdrom.c timing/cancellation semantics vs
Beetle oracle — NOT interrupts.c.

=== N+151 — PAD INPUT FIXED: OpenBIOS initCard zeroed s_padStarted ===
8754bd36 root-caused + fixed the dead-pad path (the "versus blocker"
that N+149/N+150 had reframed as CD-stall symptom — BOTH were real:
pad driver died at frame ~171 AND a separate CD stall exists). OpenBIOS
sio0Handler (prio-2 VBlank IntRP) calls readPad only if s_padStarted!=0.
BR2 calls B0:0x48 initCard with a0=0 right after PAD_init; upstream
initCard ends `s_padStarted = padStarted` (caller $a0) -> zeroed the
flag -> handler kept firing but skipped readPad forever -> tx_writes
froze @15556. Retail SCPH-1001 InitCARD has NO pad side-effects. Fix:
NOP the store @file off 0x22320 (vaddr 0xBFC22320) in openbios.bin,
documented LOCAL PATCH 2, sha256 repinned, OpenBIOS_full.c regenerated
via tool. Verified: tx_writes 15556->climbing, pad buffers update,
CROSS/START reach guest. NOTE: separate CD seek/event stall (N+150)
still under investigation by 9b4abe25 — may be the attract background
load or a real cdrom.c bug.
