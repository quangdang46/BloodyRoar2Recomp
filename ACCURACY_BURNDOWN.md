
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
