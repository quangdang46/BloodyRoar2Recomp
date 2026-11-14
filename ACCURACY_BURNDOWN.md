
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
