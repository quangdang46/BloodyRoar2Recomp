# ATTRACT freeze — root cause: apply() foe==NULL divergence relocates .ACT+4

## Symptom

ATTRACT/demo mode hard-freezes: `ev_stream_dispatch` spins forever on a
3-opcode cycle `[altstream][actsel][hitstun]` because `motion+0xa4` holds a
corrupt negative value (observed `0x859f`, `0xc59b`, `0xd33b` — different
each run because the value is a heap-address low halfword).

## Root cause

`apply()` (game/action.c, guest `func_8017A05C` @ `0x8017A05C`) seeds
`motion+0xa4` from `lh` at the `.ACT` cursor `motion+0xac`, which starts at
`act_base+4`. `.ACT+4` is the field1 **count** u32 (file value e.g. `0x1cf`/
`0x1e9`); the once-relocation pass must start at `act_base+8` so `+4` stays
raw.

The clean port NULL-checked `FC_FOE` (`ctrl+0x2c`):

```c
uint8_t *foe = foe_g ? guest_at(foe_g) : NULL;
...
if (player==0 || (foe && char != foe_char)) { self-share }
else if (foe) { mirror-share }
/* foe==NULL && player!=0 -> NEITHER path -> motion+0xac stays act_base+4 */
```

When a fighter's first `apply()` ran before foe linkage (`FC_FOE==0`,
`FC_PLAYER!=0`), cursor-share was skipped entirely, so the once-relocation
( gated by `ctrl+0xb==0`) walked from `act_base+4` and **relocated the count
field itself** (`0x1cf -> act_base+0x1cf`). Every later `apply()` then read
`lh` of the relocated pointer into `+0xa4` — a heap-dependent s16 that is
negative roughly half the time -> `ev5_action_select`'s count gate fails ->
dispatch livelock. Both versus fighters showed `.ACT+4` relocated
(`0x80085ab9`/`0x800e859f`); versus only survived because both low halves
happened positive.

## Why the guest doesn't do this

Generated `br2eu_func_8017A05C` never NULL-checks foe:

- `0x8017A0BC-0x8017A0DC`: `lw v0,0x2c(ctrl)` (FC_FOE) then
  `lbu v0,3(v0)` unconditionally — foe==0 reads guest byte @3 (=0), used to
  index the S1 table for `motion+4`.
- `0x8017A388-0x8017A3A8`: `lb v0,2(ctrl)` player check -> self path;
  else `lw a0,0x2c(ctrl)`, `lb v1,3(ctrl)`, `lb v0,3(a0)` — foe==0 reads
  guest @3 (=0) != char(>=1) -> **self path always taken** -> cursor
  advances `act_base+4 -> +8` -> once-reloc starts at `+8`, `+4` stays raw.
- Mirror tail `0x8017A3B0-0x8017A3CC` (`lw 0x38(foe)`, `lw 0xac`, `lh -4`)
  is likewise unguarded.

## Fix (game/action.c apply())

- `foe = guest_at(foe_g)` unconditionally — `guest_at(0)` is the arena
  base, reproducing the guest's low-memory reads exactly.
- `motion+4` S1 store no longer `if (foe)`-guarded (matches unconditional
  `0x8017A0BC` block).
- Cursor-share condition drops the `foe &&` guard; mirror tail dereferences
  `*(foe+0x38)+0xac` with no NULL checks (guest-faithful).

## Verification (live, port 9960, new binary)

Versus fight in progress; fighter motion records:

- P1 `0x801F5328`: `+0xa0=1`, **`+0xa4=0x000001E9`** (raw count),
  `+0xa8=0x80018904`, `+0xac=0x800858D8`, `+0xc0=1`
- P2 `0x801F5440`: **`+0xa4=0x000001CF`** (raw count), `+0xac=0x800E83D8`

Previously (old binary, same fight setup): `+0xa4=0x5AB9` / `0x859F` —
relocated `.ACT+4` low halves. Values now equal the `.ACT` file field1
counts exactly (ALI `0x1e9`, STU `0x1cf`).

Guest byte @3 confirmed `0x00` via `mem_words 0x0` -> foe==0 char index is
always 0, chars are >=1 -> self path, matching hardware.

## Status

- versus: fight running clean (frames advancing through rounds).
- attract: fresh instance cycling gs=3/4/5/6 with demo fighters loaded;
  long-run freeze watch ongoing (trigger is rare/timing-sensitive).
