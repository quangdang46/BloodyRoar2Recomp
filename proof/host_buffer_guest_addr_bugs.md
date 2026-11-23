# Host-buffer → guest-address bug class (systemic)

Clean ports keep guest-stack blocks as host C locals, then hand them to
callees that either `guest_at(param)` (expect a guest address) or
`guest_of(param)` (re-derive a guest address → 0 for host stack). Both
yield garbage/0. `brightness_packet_send` (fixed, commit 28fa473) was one
instance; a full audit found 4 more high-confidence clusters.

Uniform fix direction: back the block with `br2_guest_frame` (established
pattern — `hp_bar_tick` menu.c, `print_dispatch` game.c,
`memcard_block_verify`, `cd_file_load_sync`, `brightness_packet_send`) and
pass the guest address / `guest_at(guest_addr)` to callees. For callback
dispatch, route through `call_guest`/clean_table so the guest-address ABI
is honored.

## Confirmed bugs

### 1. Stepper-descriptor callback ABI break — SYSTEMIC (6 sites)

Menu/save "stepper" functions pass a host stack descriptor to
`menu_stepper_state_advance`/`button_wait`/`redraw`/`masked_sweep`. The
`+0xc` callback field is a guest code address resolved via `fn_lookup`;
dispatch is inconsistent:

- `cb(param_1, code)` passes the HOST pointer (works only for
  host-pointer callees like `attract_menu_dispatch`).
- `call_guest(cb_g, guest_of(param_1), code)` → `guest_of(host stack)=0`
  → generated callees get `a0=0`.

Callees that `guest_at(a0)` REQUIRE a guest address and read garbage:
`dialog_sel_dispatch` (save.c:3543), `dialog_sel_dispatch_wide`
(save.c:5529), `dialog_msg_dispatch` (save.c:6869).

| Caller | desc decl | cb (+0xc) | callee | failure |
|---|---|---|---|---|
| save.c:784 memcard_file_flow | step_desc[0x24] @681 | 0x80160598 @742 | dialog_sel_dispatch | host ptr→guest_at(garbage) |
| menu.c:1054 system_config_menu_a | step_desc_a[0x30] @994 | 0x8015caa8 @1085 | dialog_msg_dispatch | same |
| menu.c:1218 system_config_menu_b | step_desc_b[0x28] @1169 | 0x8015bdc0 @1266 | dialog_sel_dispatch_wide | same |
| save.c:3500 menu_stepper_sm | desc[0x38] @3438 | 0x8015b190 @3453 (unported) | call_guest | a0=0 |
| menu.c:13610 save_flow_state_machine | blk[0x28] @13547 | 0x8015d69c @13566 | call_guest | a0=0 |
| menu.c:13690 options_state_machine | blk[0x28] @13624 | 0x8015ee40 @13647 | call_guest | a0=0 |
| menu.c:13753 screen_flow_state_machine | blk[0x28] @13703 | 0x80160a64 @13722 | call_guest | a0=0 |

Near-miss: game.c:1507 match_loop→menu_stepper_state_advance(&sd) with
callback 0x8014751c attract_menu_dispatch (host-ptr callee) — works only
because both sides use host pointers; fragile.

### 2. sprite_slot_commit — host blk[2] → gpu_texclut_pkt (menu.c)

menu.c:10301 `uint32_t blk[2]` → menu.c:10308
`gpu_texclut_pkt(guest_at(...), blk, ...)`. Callee gfx.c:2695 does
`guest_of(a1)` → 0 for host stack → falls back to truncated host ptr as
guest addr → `guest_at` returns garbage → `t0[3]=a1[0]` etc read wrong.
Silent wrong GPU texclut packet.

### 3. hud_announce — host buf[16] → descblk_variant_e (save.c)

save.c:6462 `char buf[16]` → save.c:6499/6565 `descblk_variant_e(..., buf)`.
Callee menu.c:13422 does `MW32(0x1f8003a8, guest_of(param_3))` → 0 →
text-prim string ptr = guest 0 → announce text renders garbage.
Fix like `hud_time_score_draw` (battle.c:16515 auStack_48 guest frame).

### 4. player_name_entry — host local_60/local_78 → announce_param_block_init_alt (menu.c)

menu.c:1616 `local_60[8]` + menu.c:1605 `local_78` →
menu.c:1741/1757 `announce_param_block_init_alt`. Callee battle.c:16486
`BA32(0x1f8003a8)=guest_of(param_3)` → 0. Sibling call menu.c:1726 uses
guest-frame `auStack_58` (correct) — only these two unbacked.

## Cleared near-misses

cd_cmd_bytes/retry (fixed), cd_file_open, slot_status_fill,
seg_sphere_hit/sphere_pt_hit, isqrt_refine, print_dispatch family,
stream_block_decode, gpu_texclut_pkt t0 arg, task_node_alloc,
angle_pair_rotmat, heap_serialize, name_rec_*, fighter_reinit_tail,
gpu_load_image/clear_image_cmd. See audit report for full list.

## Latent (LOW)

- battle.c:16835 LAB_80137acc clean `fn` invoked with host ptr while
  generated fallback gets guest addr — harmless now (all table targets
  take void), breaks if a guest-addr handler lands there.
- battle.c:11046 task_slot_fns_run passes host local_10 — same caveat.
