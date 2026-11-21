# x28-clobber root cause — PROOF

## Verdict

**NOT an ABI / IRQ / setjmp bug. A clean-C stack-buffer overflow in
`fighter_pair_update` (game/battle.c) smashed the host-saved `x28` slot.**

The decompiler split each 8-byte guest hitbox-result buffer into a 2-short
array + a trailing scalar. Callees write result shorts at indices 0..2
(6 bytes); index 2 ran off the end of the 4-byte array and overwrote
`fighter_pair_update`'s saved `x28` on the host stack. The corrupted `x28`
was restored by `fighter_pair_update`'s epilogue and propagated into
`battle_flow_state_machine`, where `ldr x3,[x25/x28]` fed a garbage
`param_4` to `fighter_flash_update` → SIGSEGV.

The scary-looking bad value (`g_irqctx_ring + 454644`) was a COINCIDENCE:
it is simply `&g_DAT_1f8001f0` (0x11128aec8) with its low 16 bits overwritten
by a guest short (0xaec8 → 0x0494 = 0x111280494), which happens to land
inside the 448 KB `g_irqctx_ring` BSS array. No IRQ/longjmp/fiber was
involved — the earlier "x28 = ring pointer" observation was this same
16-bit partial overwrite landing in ring address space.

## Runtime evidence (lldb, port 5599, instrumented build)

Temporary `x28_chk(site)` instrumentation in `battle_flow_state_machine`
(game/menu.c) latches the first site where the callee-saved pointer regs
deviate. NOTE: this build's regalloc keeps **x25 = &g_DAT_1f8000e8** (P1)
and **x28 = &g_DAT_1f8001f0** (P2) — verified in disasm:

```
0x10165e7c8 <+160>:  add x25, x25, #0xe78 ; g_DAT_1f8000e8
0x10165e7d0 <+168>:  add x28, x28, #0xec8 ; g_DAT_1f8001f0
0x10165ecc4 <+1436>: ldr x3, [x25]   ; P1 fighter_flash_update arg
0x10165f33c <+3092>: ldr x3, [x28]   ; P2 fighter_flash_update arg
```

Post-mortem globals after the versus fight fail-fast:

```
g_x28_bad_site = 6        (x28_chk(6) — immediately AFTER fighter_pair_update)
g_x28_bad_reg  = 28
g_x28_bad_val  = 0x111280494  = g_irqctx_ring + 454644
                = &g_DAT_1f8001f0 (0x11128aec8) with low16 0xaec8 -> 0x0494
```

Site map (game/menu.c): chk(5) is after the 2nd `FUN_801787dc`, then
`fighter_pair_update(...)`, then chk(6). chk(5) passed, chk(6) failed →
the clobber happened **inside `fighter_pair_update`**.

## Stack-layout proof

`fighter_pair_update` (host disasm, build before fix):

```
+4:   stp x28, x27, [sp, #0x10]     ; saved x28 at sp+0x10
+356: add x2, sp, #0xc              ; hitbox_pair_test(P1,P2, local_38 @ sp+0xc)
+460: add x2, sp, #0x8              ; hitbox_pair_test(P2,P1, local_30 @ sp+0x8)
+2592: ldp x28, x27, [sp, #0x10]    ; epilogue restores (corrupted) x28
```

`hitbox_pair_test(param_1,param_2,param_3)` (game/battle.c:3499) writes
THREE shorts through param_3:

```c
*local_30     = ...;   // param_3[0]
local_30[1]   = ...;   // param_3[1]
local_30[2]   = ...;   // param_3[2]  <- 6 bytes total
(seg_sphere_hit / sphere_pt_hit also write param_6[1]/[2], param_5[1]/[2])
```

Call 1 passes `sp+0xc`; `param_3[2]` = `sp+0x10` = **saved x28 slot**.
The written value is a guest-space hit coordinate short (here 0x0494),
so restored x28 = 0x11128_0494 — exactly the observed bad value.

## Guest (authoritative) layout — generated/SLES_017.22_full_37.c

`br2eu_func_801809C8` (guest 0x801809C8, fighter_pair_update):

```
0x801809C8: addiu sp,sp,-0x50            ; 80-byte frame
0x80180B1C: a2 = sp + 24  (0x18)         ; first hitbox_pair_test buffer
0x80180B70: a2 = sp + 32  (0x20)         ; second hitbox_pair_test buffer
```

The two result buffers are **8-byte slots** (sp+0x18 and sp+0x20), giving
callee writes at +0,+2,+4 bytes room to spare. The clean port collapsed
each into `short x[2]` (4 bytes) + separate scalars → overflow.

## The fix (game/battle.c, fighter_pair_update ~line 666)

```c
short local_38 [4];   /* was local_38[2] + local_34 — guest sp+0x18, 8B slot */
short local_30 [4];   /* was local_30[2] + local_2c — guest sp+0x20, 8B slot */
```

with `local_34` → `local_38[2]` and `local_2c` → `local_30[2]` at all use
sites (node_translate_tree z-args, position accumulators). This is both
the memory-safety fix AND a semantic fix: previously the index-2 result
z-coordinate was lost to the overflow (local_34 never received it), so
hit reactions/wall-collide positions were computed from stale z.

## Verification

Rebuilt (`cmake --build build`), re-ran versus to ph=8 on port 5601:
**`g_x28_bad_site = 0` — no callee-saved corruption through the whole
fight.** The fighter_flash_update SIGSEGV is gone.

## Remaining issue (SEPARATE, not the x28 bug)

The versus fight still ends in `FAIL-FAST unknown dispatch: addr=0x01000000
ra=0x80126664` (guest wild jump; ra = return site after
battle_flow_state_machine in the g_game_state SM, game/game.c case 8).
gpr[8]=gpr[26]=0x01000000 suggests a corrupted guest function pointer /
`jr` target — likely ANOTHER instance of the decompiler local-split
buffer bug (or unrelated versus-path bug) corrupting guest state rather
than host regs. Needs its own investigation.

## Files changed

- game/battle.c — fighter_pair_update buffer fix (the actual fix)
- game/menu.c — TEMP x28_chk instrumentation (still present; remove when
  the fail-fast hunt is done)

## Artifacts

- proof/x28hunt/nav5599.log, nav5601.log — versus runs
- /tmp + lldb transcripts in shell-lldbint2-*/content.txt
- generated/SLES_017.22_full_37.c:6187+ (guest fighter_pair_update)
- generated/SLES_017.22_full_37.c:~7290-7330 (jal 0x80145970 sites, a2=sp+0x18/0x20)
