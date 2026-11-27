/* cd.c — CD/file services: async read pump, status pollers,
 * command issuer with retry (verbatim Ghidra, decomp/batch_sm3.txt).
 *
 * Guest globals via g_* arena bindings; timer cd_timer_read and
 * channel ops mc_session/95d78/961a8/961cc/98590/95ec8/98908/
 * 96e14/98c90 remain extern (deeper CD-driver cluster). */
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cpu_state.h"

typedef uint8_t  uint8_t;
#ifndef bool
#define bool int
#endif

extern void *guest_at(uint32_t addr);
extern uint32_t guest_sp(void);
extern uint32_t psx_read_word(uint32_t addr);
extern void psx_write_word(uint32_t addr, uint32_t val);

uint32_t cd_timer_read(int);
void cd_busy_wait_fwd(int);
int cd_cmd_dispatch3(uint32_t);
void cd_init_done_cb(void);
uint32_t mc_session(uint32_t);
uint32_t mc_session_b(uint32_t);
uint32_t cd_cmd_issue_async(int8_t,int);
uint32_t cd_read_open(int);
uint32_t cd_cmd_issue(int8_t,uint32_t,uint32_t,int);
bool cd_read_setup(uint32_t,uint32_t,uint32_t);
int cd_chan_boot_init(int);
void cd_result_wait_b_fwd(int,uint32_t);
void printf_forward(const char *, ...);
extern void br2_irq_poll(void);
extern int br2_bail_pending(void);
extern uint32_t br2_guest_frame(uint32_t);
extern void br2_guest_frame_free(void);
extern void br2_guest_frame_abandon(void);
extern CPUState *clean_cpu_state(void);
extern int clean_try_dispatch(CPUState *cpu, uint32_t addr);
extern void psx_dispatch_call(CPUState *cpu, uint32_t target_addr,
                              uint32_t return_addr);

/* cd_jal — faithful guest `jal`/`jalr` from clean C (same contract as
 * bind.c's bind_jal): args are already live in cpu->gpr[4..7] (the caller
 * writes them explicitly), caller-saved regs keep callee leftovers exactly
 * like the guest (NO arg-reg restore — call_guest/call_guest4 would restore
 * a0/a1 and feed downstream BIOS tramps stale args), $ra is set to the
 * guest link address so BIOS/kernel services inside the callee see the
 * same caller token the generated CPS frame presents, and $v0 comes back
 * in gpr[2]. clean targets run through clean_try_dispatch; everything else
 * goes through psx_dispatch_call. */
static uint32_t cd_jal(uint32_t fn, uint32_t link)
{
    CPUState *cpu = clean_cpu_state();
    uint32_t ra, pc, sp;
    if (!cpu || fn == 0) return 0;
    if (br2_bail_pending()) return cpu->gpr[2];
    ra = cpu->gpr[31]; pc = cpu->pc; sp = cpu->gpr[29];
    cpu->gpr[31] = link;
    if (clean_try_dispatch(cpu, fn)) {
        uint32_t r = cpu->gpr[2];
        cpu->gpr[31] = ra; cpu->pc = pc;
        return r;
    }
    psx_dispatch_call(cpu, fn, link);
    cpu->gpr[29] = sp;
    {
        uint32_t r = cpu->gpr[2];
        cpu->gpr[31] = ra; cpu->pc = pc;
        return r;
    }
}

/* Diagnostic: clean-C CD function entry counters. These functions access guest
 * state through DB32()/guest_at() host pointers, which bypass the generated
 * store path — so their calls AND their writes to cells like 0x801bc84c are
 * invisible to wtrace/fntrace. These counters (dumped via debug "cdfn_dump")
 * are the only observability into that lifecycle. Index = CDFN_* below. */
extern uint32_t g_cdfn_log[24];   /* defined in runtime interrupts.c */
extern uint32_t g_cdfn_arg[24];
#define CDFN(n,a) do { g_cdfn_log[(n)]++; g_cdfn_arg[(n)]=(uint32_t)(a); } while(0)
/* bc84c write sequence: mc_session_b is the ONLY clean-C writer of the SM-driver
 * cell; generated writers (mode_set_0e sm_wrap install, chan_reset_b clear) are
 * invisible here. Record (new,old) pairs to see the park/install/restore/clear
 * ordering that decides whether sm_wrap survives. Ring of 96 pairs. */
extern uint32_t g_bc84c_seq[192];
extern uint32_t g_bc84c_seq_n;
extern uint32_t g_bc84c_caller[64];
enum {
  CDFN_READ_SETUP=0, CDFN_INIT_DONE_CB, CDFN_READ_FINISH_CB,
  CDFN_SUBSYS_RESET, CDFN_SUBSYS_INIT, CDFN_DIR_TABLE_SET,
  CDFN_MODE_SET_0E, CDFN_DIR_RESET, CDFN_MODE_APPLY_DEFERRED,
  CDFN_MC_SM, CDFN_CHAN_RESET_B, CDFN_INIT_MODE_DISPATCH,
  CDFN_SEEK_STEP, CDFN_TILE_STREAM_PUMP, CDFN_STREAM_MODE_INIT,
  CDFN_FILE_LOAD_SM, CDFN_MC_SESSION_B, CDFN_READ_OPEN,
  CDFN_RESP_PUMP, CDFN_BLK_IO_RETRY, CDFN_READ_STEP_CB,
  CDFN_INIT_SESSION, CDFN_PROBE_RETRY, CDFN_MODE_FLAG_SET
};
extern uint32_t guest_of(const void *);
extern uint32_t br2_gaddr(const void *);
extern uint8_t  br2_ld8p(uint32_t cell);
extern uint16_t br2_ld16p(uint32_t cell);
extern uint32_t br2_ld32p(uint32_t cell);

/* DIAG: ring recording every write to resume stamp 0x801feb00.
 * tag 1 = cd_dir_entry_finalize (fc6), tag 2 = cd_dir_entry_store (bc6). */
#define FEB00_TRACE_CAP 2048
static uint32_t s_feb00_val[FEB00_TRACE_CAP];
static uint32_t s_feb00_tag[FEB00_TRACE_CAP];
static uint32_t s_feb00_seq;
static void feb00_note(uint32_t tag, uint32_t val) {
  uint32_t i = s_feb00_seq % FEB00_TRACE_CAP;
  s_feb00_val[i] = val;
  s_feb00_tag[i] = tag;
  s_feb00_seq++;
}
/* cd_feb00_trace_dump — format feb00 trace ring (tag:val; CSV) into out, cap-bounded; ret chars written. */
int cd_feb00_trace_dump(char *out, int cap) {
  int pos = 0, i, n = (int)(s_feb00_seq < FEB00_TRACE_CAP ? s_feb00_seq : FEB00_TRACE_CAP);
  int start = (int)(s_feb00_seq - (uint32_t)n);
  for (i = 0; i < n && pos < cap - 48; i++) {
    uint32_t idx = (uint32_t)(start + i) % FEB00_TRACE_CAP;
    pos += snprintf(out + pos, (size_t)(cap - pos), "%s%u:%08x;",
                    i ? "," : "", s_feb00_tag[idx], s_feb00_val[idx]);
  }
  return pos;
}
/* cd_feb00_trace_total — DIAG: total entries recorded in the
 * cd_feb00_trace ring (see cd_feb00_trace_dump). */
uint32_t cd_feb00_trace_total(void) { return s_feb00_seq; }

/* DIAG: ring recording CD seek-position resolutions.
 * tag 3 = cd_file_open_b (0x801feb00 stamp -> sector),
 * tag 4 = cd_stream_mode_init (stamp buf -> sector). */
#define SEEK_TRACE_CAP 4096
static uint32_t s_seek_a[SEEK_TRACE_CAP];
static uint32_t s_seek_b[SEEK_TRACE_CAP];
static uint32_t s_seek_tag[SEEK_TRACE_CAP];
static uint32_t s_seek_seq;
void cd_seek_note(uint32_t tag, uint32_t a, uint32_t b) {
  uint32_t i = s_seek_seq % SEEK_TRACE_CAP;
  s_seek_tag[i] = tag; s_seek_a[i] = a; s_seek_b[i] = b; s_seek_seq++;
}
/* cd_seek_dump — format seek trace ring (tag:a>b; CSV) into out; ret chars written. */
int cd_seek_dump(char *out, int cap) {
  int pos = 0, i, n = (int)(s_seek_seq < SEEK_TRACE_CAP ? s_seek_seq : SEEK_TRACE_CAP);
  int start = (int)(s_seek_seq - (uint32_t)n);
  for (i = 0; i < n && pos < cap - 40; i++) {
    uint32_t idx = (uint32_t)(start + i) % SEEK_TRACE_CAP;
    pos += snprintf(out + pos, (size_t)(cap - pos), "%s%u:%08x>%u;",
                    i ? "," : "", s_seek_tag[idx], s_seek_a[idx], s_seek_b[idx]);
  }
  return pos;
}
/* cd_seek_total — DIAG: total entries recorded in the cd_seek_trace
 * ring (see cd_seek_trace_dump). */
uint32_t cd_seek_total(void) { return s_seek_seq; }
extern void     br2_st8p(uint32_t cell, uint8_t v);
extern void     br2_st16p(uint32_t cell, uint16_t v);
extern void     br2_st32p(uint32_t cell, uint32_t v);
extern uint8_t  br2_ld8(uint32_t a);
extern uint16_t br2_ld16(uint32_t a);
extern uint32_t br2_ld32(uint32_t a);
extern void     br2_st8(uint32_t a, uint8_t v);
extern void     br2_st16(uint32_t a, uint16_t v);
extern void     br2_st32(uint32_t a, uint32_t v);
extern uint32_t br2_call_generated(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern uint32_t call_guest4(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern uint32_t bios_svc6(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern uint32_t clean_caller_ra(void);
extern void     br2_guest_ra_set(uint32_t);

/* MMIO register-block accessors. The pointer cells 0x801bdeb0,
 * 0x801bdec0 and 0x801bdec4 resolve to HARDWARE addresses — the SPU
 * register file (0x1F801C00+), DPCR (0x1F8010F0) and 0x1F801014.
 * guest_at() can only back RAM/scratchpad; an MMIO address lands in the
 * open-bus mirror so the access never reaches the device (the same
 * mistranslation class as the GPU/DMA pointer cells fixed earlier).
 * Every register-block access must go through br2_ldN/br2_stN on the
 * resolved guest address. idx is the u16 index the original b[idx]
 * accesses used (int8_t offset = idx*2). */
#define CDMMIO16(base, idx)        br2_ld16((base) + ((uint32_t)(idx) << 1))
#define CDMMIO16_ST(base, idx, v)  \
    br2_st16((base) + ((uint32_t)(idx) << 1), (uint16_t)(v))
#define CDMMIO32(base, idx)        br2_ld32((base) + ((uint32_t)(idx) << 2))
#define CDMMIO32_ST(base, idx, v)  \
    br2_st32((base) + ((uint32_t)(idx) << 2), (uint32_t)(v))

/* ---- guest globals ----
 * The g_DAT_* bindings live in bind.c (guest_for registry) — names are
 * load-bearing, do NOT rename them here. The CD_* macros below are the
 * readable aliases; guest address kept in each comment. */
extern uint8_t  *g_DAT_801bc7b4;
extern uint8_t  *g_DAT_801bcb80;
extern uint8_t  *g_DAT_801bcb94;
extern uint8_t  *g_DAT_801bcb98;
extern uint8_t  *g_DAT_801bcb9c;
extern uint8_t  *g_DAT_801bcba4;
extern uint8_t  *g_DAT_801bcba8;
extern uint8_t  *g_DAT_801bcbac;
extern uint8_t  *g_DAT_801bcbb0;
extern uint8_t  *g_DAT_801bc854;
extern uint8_t  *g_DAT_801bc848;
extern uint8_t  *g_DAT_801bc7c0;

#define CD_DISPATCH_PEND   (*(uint8_t  *)g_DAT_801bc7b4)  /* u8 @0x801bc7b4: bit0 = dispatch pending */
#define CD_RD_COUNT        (*(int32_t  *)g_DAT_801bcb80)  /* i32 @0x801bcb80: read sector count */
#define CD_RD_RESULT       (*(int32_t  *)g_DAT_801bcb94)  /* i32 @0x801bcb94: read result/remaining (-1 err) */
#define CD_RD_TS_ISSUE     (*(int32_t  *)g_DAT_801bcb98)  /* i32 @0x801bcb98: tick after ReadN issue */
#define CD_RD_TS_PAUSE     (*(int32_t  *)g_DAT_801bcb9c)  /* i32 @0x801bcb9c: tick after Pause/issue */
#define CD_RD_DONE         (*(int32_t  *)g_DAT_801bcba4)  /* i32 @0x801bcba4: read-session done flag */
#define CD_RD_SESS_A       (*(int32_t  *)g_DAT_801bcba8)  /* i32 @0x801bcba8: mc_session handle */
#define CD_RD_SESS_B       (*(int32_t  *)g_DAT_801bcbac)  /* i32 @0x801bcbac: mc_session_b handle */
#define CD_RD_DISP         (*(int32_t  *)g_DAT_801bcbb0)  /* i32 @0x801bcbb0: cd_cmd_dispatch3 result */
#define CD_STATUS          (*(uint8_t *)g_DAT_801bc854)   /* u8 @0x801bc854: raw CD status (cd_status_raw) */
#define CD_BIOS_CB_20      (*(uint32_t *)g_DAT_801bc848)  /* u32 @0x801bc848: fn ptr -> cd_bios_cmd20 */
#define CD_CMD_TBL         ((int32_t  *)g_DAT_801bc7c0)   /* i32* @0x801bc7c0: per-opcode table */

/* ======== 0x8018d150 ======== */

uint32_t cd_mode_flag_set(int8_t,int,uint32_t);
uint32_t cd_cmd_issue(int8_t,uint32_t,uint32_t,int);

uint32_t cd_read_issue(uint32_t a0,uint32_t a1,uint32_t a2)

{
  /* IRQ/coroutine-boundary function: clean-C body cannot supply the
   * live guest register file the deferred CD coroutine needs, and its
   * MMIO busy-waits lack per-block interrupt checks. Delegate to the
   * generated MIPS (behavioral oracle) — same contract as the other
   * read-path functions already delegated. */
  return (uint32_t)call_guest4(0x8018d150u,a0, a1, a2, 0);
}

/* ======== 0x80198af0 ======== */

/* cd_init_session — CD read-session init + async-Pause handshake (0x80198af0).
 * Waits on CD_RD_DONE (cleared by session cb 0x80198590 = cd_init_done_cb,
 * registered as the INT2 command-complete callback via mc_session->
 * CD_BIOS_CB_20). It issues Pause(0x09) once via cd_cmd_issue_async, sets
 * CD_RD_DONE=1, then waits <=0x79 frame ticks (cd_timer_read -> 0x801bdde0
 * VBlank counter) for the INT2 cb to clear the flag.
 *
 * This wait MUST run the generated body: the deferred-IRQ ctx save/restore
 * (cd_ctx_save/cd_ctx_restore + cd_intc_dispatch) needs LIVE cpu->gpr, which a
 * clean-C spin cannot provide — br2_irq_poll delivers the IRQ against stale
 * cpu->gpr -> cd_intc_dispatch wild store @0x8019AA98 (and the frame counter
 * freezes with no per-block interrupt check, so the 0x79 timeout never fires
 * -> infinite 100% CPU spin + repeated cmd=0x09 as boot retries init).
 * Same contract as cd_chan_full_init/cd_ctx_save (commit 15716b2): the
 * br2_call_generated thunk also makes gen_clean_table skip this address. */
/* cd_init_session — delegate whole body to generated MIPS 0x80198AF0. */
void cd_init_session(void)
{
  call_guest4(0x80198af0, 0, 0, 0, 0);
}

/* ======== 0x80198e2c ======== */

/* cd_read_poll — CD read-session poll (0x80198e2c, 18 blocks verified
 * insn-by-insn against generated/SLES_017.22_full_46.c:7029).
 *
 * Session struct base s1 = 0x801BCB9C; s2 = 0x801BCBA4 (completion flag,
 * set to 1 by cd_read_finish_cb, cleared by cd_init_done_cb). Loop:
 *   E5C  jal cd_timer_read(-1)          ; delay slot: s0 = -1 (every pass)
 *   E64  if ((int32)(bcb9c + 0x4B0) < now) -> exit with s0 = -1
 *   E7C  if ((int32)bcb94 < 0) -> reopen
 *   E8C  else jal cd_timer_read(-1); if ((int32)(bcb98 + 0x3C) < now2)
 *        -> reopen; else s0 = bcb94
 *   EAC  reopen: jal cd_read_open(1); s0 = bcb80
 *   EC4  if a0 != 0 -> exit
 *   ECC/EDC/EE4: continue iff s0 > 0 || (bcba4 != 0 && s0 == 0)
 *   EEC  exit: jal cd_result_wait_b_fwd(1, a1)
 *   EF4  v0 = bcba4; return bcba4==0 ? 0 : (s0!=0 ? bcba4 : 1)
 *        (bcba4 is only ever 0/1 -> effectively returns bcba4 != 0)
 *
 * The faithful CFG port (br2_irq_poll + br2_bail_pending per iteration,
 * both nested jals delegated) compiled and ran ~1519 frames, then died
 * deterministically: FAIL-FAST unknown dispatch 0x8019A5D8 (vsync_wait_n)
 * with ra=0x801265F4. Post-mortem (gwatch mprotect watch + decomp_trace)
 * showed the poll loop's IRQ delivery is NOT equivalent to the generated
 * per-block psx_check_interrupts_at: the CD deferred coroutine
 * (cd_ctx_restore -> cd_chan_full_init -> cd_intc_dispatch) resumes
 * against stale cpu->gpr, the INT2 completion for cd_file_load_sm(0x13)
 * (battle-intro resource load via battle_intro_flow -> menu_res_load_sm
 * -> save_channel_init_commit) lands wrong/late, the tag-table file
 * region keeps stale host-pointer garbage, stream_block_decode ->
 * mode3_dispatch -> block_copy_ret_len decodes host pointers into heap
 * blocks at ~0x801B563C, and data_relocate's fixup loops then spray
 * `+= base` writes over ~600KB of guest RAM — overwriting code at
 * 0x8019A5D8 and the frame counter at 0x801BDDE0 (both verified via
 * gwatch rip->save.c:9623 and read_ram vs master). This is exactly the
 * §4 deferred-coroutine starvation class: a clean-C poll loop cannot
 * supply the live $ra/resume-PC the generated CPS frame provides, so
 * IRQ-adjacent waits on the CD read path must stay delegated.
 * Faithful port preserved in git history (stash "cd_read_poll port wip"). */
int cd_read_poll(int a0,uint32_t a1)
{
  (void)a0; (void)a1;
  return (int)call_guest4(0x80198e2cu, (uint32_t)a0,
                                 (uint32_t)a1, 0, 0);
}

/* ======== 0x80195d8c ======== */

extern int cd_stamp_to_sector(const void *);
uint32_t cd_mode_flag_set(int8_t opcode,int param_addr,uint32_t param_val)

{
  /* Faithful port of 0x80195d8c-0x80195ec4: bounded 4-attempt retry loop
   * around cd_cmd_issue (delegated generated — supplies the per-block
   * interrupt checks / deferred-coroutine resume inside each call, same
   * contract as cd_cmd_issue_sync 0x80195ffc which has the identical
   * shape). CD_BIOS_CB_20 handshake matches the guest exactly: cleared at
   * each attempt head, restored before the main issue and once more on
   * exhaustion — but NOT restored on issue(2)-failure iterations (guest
   * branches straight to the decrement). */
  uint32_t s3 = (uint32_t)(uint8_t)opcode;
  uint32_t saved_cb = CD_BIOS_CB_20;
  int retries = 3;
  do {
    int do_main = 1;
    CD_BIOS_CB_20 = 0;
    if ((s3 != 1) && ((CD_STATUS & 0x10) != 0)) {
      cd_cmd_issue(1,0,0,0);
    }
    if ((param_addr != 0) && (CD_CMD_TBL[s3] != 0)) {
      if (cd_cmd_issue(2,param_addr,param_val,0) != 0) do_main = 0;
    }
    if (do_main) {
      CD_BIOS_CB_20 = saved_cb;
      if (cd_cmd_issue(opcode,param_addr,param_val,0) == 0) return 1;
    }
    retries = retries - 1;
  } while (retries != -1);
  CD_BIOS_CB_20 = saved_cb;
  return 0;
}

/* ======== 0x8019b200 ======== */

/* cd_chan_boot_fwd — boot-init channel 0 via cd_chan_boot_init(0). */
void cd_chan_boot_fwd(void)

{
  cd_chan_boot_init(0);
  return;
}

#define CD_RD_POS    (*(uint32_t *)guest_at(0x801bcb84))  /* u32 @0x801bcb84: read setup param2 (pos/aux) */
#define CD_RD_CUR    (*(int32_t  *)guest_at(0x801bcb88))  /* i32 @0x801bcb88: active read position (copy of POS) */
#define CD_RD_MODE   (*(uint32_t *)guest_at(0x801bcb8c))  /* u32 @0x801bcb8c: Setmode param int8_t (|0x20) */
#define CD_RD_SECTSZ (*(uint32_t *)guest_at(0x801bcb90))  /* u32 @0x801bcb90: sector size 0x200/0x246/0x249 */
#define CD_BIOS_CB_40 (*(uint32_t *)guest_at(0x801bc84c)) /* u32 @0x801bc84c: fn ptr -> cd_bios_cmd40 */
#define CD_RD_SECTOR (*(uint32_t *)guest_at(0x801bcba0))  /* u32 @0x801bcba0: read start sector */

/* ======== batch26: CD driver cluster ======== */

extern uint32_t cd_result_wait_a(int,uint8_t *);
extern void cd_str_print(uint32_t);
extern void cd_cmd_state_reset(void);
extern uint16_t cd_intc_busy_read(void);
extern uint32_t cd_response_process(void);
extern uint8_t cd_status_raw(void);
extern bool cd_cmd_issue_sync(int8_t,int,uint32_t);
extern uint8_t * cd_param_buf_get(void);
extern void cd_cmd_reset_wrap(void);
extern uint8_t cd_mode_byte_read(void);
extern int cd_stamp_to_sector(const void *);
extern void vsync_wait_n(int,int);
extern uint32_t cd_result_wait_b(int,uint8_t *);
extern uint32_t cd_busy_wait(int);
extern int cd_dispatch_tbl4(int,uint32_t);
uint32_t call_guest(uint32_t,uint32_t,uint32_t);

extern char *s__s____80125840;
extern char *s_CD_timeout__801257a4;
extern char *s__s___s__Sync__s__Ready__s_801257b4;
extern char *s__s__no_param_80125848;
extern char *s_CD_cw_80125858;
extern char *s_CdRead__Shell_open____80125ae0;
extern char *s_CdRead__retry____80125af8;

/* pointer-valued globals: guest stores a guest addr; deref via guest_at */
#define DB16P(a) br2_ld16p(a)
#define DB8P(a)  br2_ld8p(a)
#define DB32P(a) br2_ld32p(a)
#define DB32G(a) (*(uint32_t *)guest_at(a))
#define DB8(a)   (*(uint8_t  *)guest_at(a))
#define DB16(a)  (*(uint16_t *)guest_at(a))
#define DB32(a)  (*(uint32_t *)guest_at(a))
#define I32AT(a,i) (((int32_t *)guest_at(a))[(i)])

/* cd_cmd_issue — CD command issue. arg_addr/arg_addr2 are guest addrs
 * (or 0); async_flag = async flag. */
uint32_t cd_cmd_issue(int8_t opcode,uint32_t arg_addr,uint32_t arg_addr2,int async_flag)

{
  /* The sync wait loop (DB8(0x801bcb24)==0) depends on the CD interrupt
   * handler setting the intc-busy flag that gates cd_response_process().
   * A clean C spin cannot be interrupted at guest-instruction granularity,
   * so the IRQ that would set busy is either deferred forever or abandons
   * this frame. Run the recompiled guest body so the wait is interruptible
   * exactly like hardware. Clean port preserved in git history. */
  return (uint32_t)call_guest4(0x80196e14,(uint32_t)opcode,arg_addr,arg_addr2,(uint32_t)async_flag);
}

/* ======== 0x80198c90 ======== */

/* cd_read_setup — CD read-setup entry; delegates to generated MIPS (IRQ-timing bound). */
bool cd_read_setup(uint32_t count,uint32_t pos,uint32_t mode)

{
  /* Read configurator: picks sector size (0x200/0x246/0x249), mode|0x20,
   * buffer ptrs, registers the INT2/step callbacks via mc_session(_b),
   * arms the 0x79 timeout and issues Pause when the drive reports busy.
   * A wrong sector-size/mode/buffer or a callback registered against the
   * wrong slot makes the read deliver the wrong amount -> the consumer
   * stops mid-sector and the completion event never fires (0x60a wedge).
   * It also sits on the IRQ/deferred-coroutine boundary like the other
   * read-path functions already delegated. Delegate to generated. */
  CDFN(CDFN_READ_SETUP,(uint32_t)count);
  return (bool)call_guest4(0x80198c90u,(uint32_t)count,
                                  (uint32_t)pos,(uint32_t)mode,0);
}

#if 0
  /* DEAD — superseded clean-port attempt of the CD read-setup body
   * (0x80198C90 region): kept for reference only; the live path above
   * delegates via call_guest4 because the clean-C spin could not supply
   * the live $ra/resume-PC the generated CPS frame provides (see the
   * cd_read_poll note). Do not re-enable without re-verifying IRQ timing. */
  int t1;
  int t2;
  uint32_t status;

  if (CD_RD_DONE != 0) {
    t1 = cd_timer_read(0xffffffff);
    do {
      if (CD_RD_DONE == 0) goto L_80198d0c;
      t2 = cd_timer_read(0xffffffff);
    } while ((uint32_t)(t2 - t1) < 0x79);
    mc_session(CD_RD_SESS_A);
    CD_RD_DONE = 0;
  }
L_80198d0c:
  if ((a2 & 0x30) == 0) {
    CD_RD_SECTSZ = 0x200;
  }
  else if ((a2 & 0x30) == 0x20) {
    CD_RD_SECTSZ = 0x249;
  }
  else {
    CD_RD_SECTSZ = 0x246;
  }
  CD_RD_MODE = a2 | 0x20;
  CD_RD_COUNT = a0;
  CD_RD_POS = a1;
  CD_RD_SESS_A = mc_session(0);
  CD_RD_SESS_B = mc_session_b(0);
  if ((CD_DISPATCH_PEND & 1) != 0) {
    CD_RD_DISP = cd_cmd_dispatch3(0);
  }
  CD_RD_TS_PAUSE = cd_timer_read(0xffffffff);
  status = cd_status_raw();
  if ((status & 0xe0) != 0) {
    cd_cmd_issue_sync(9,0,0);
  }
  t1 = cd_read_open(0);
  return 0 < t1;
}
#endif

/* ======== 0x8019a460 ======== */

/* cd_timer_read — frame/timer read + DAT_801bdde0 waits (0x8019a460).
 * Faithful port of generated 0x8019A460 (19 blocks, verified insn-by-insn).
 * Samples the tick counter *(*0x801bccac) under a seqlock against
 * *0x801bccb0, derives s1 = (snap - *0x801bccb0) & 0xffff, then:
 *   a0 < 0  -> return *0x801bdde0 (VBlank frame count)
 *   a0 == 1 -> return s1
 *   a0 == 0 -> vsync_wait_n(*0x801bccb4, 0)
 *   a0 >= 2 -> vsync_wait_n(*0x801bccb4 + a0 - 1, a0 - 1)
 * then vsync_wait_n(*0x801bdde0 + 1, 1), a sign-bit flip wait on the
 * *(*0x801bcca8) counter (only when the pre-wait sample has bit
 * 0x00400000 set), and a second seqlock publish of the tick into
 * 0x801bccb0. Returns s1.
 *
 * Both `jal vsync_wait_n` sites dispatch to the CLEAN body below
 * (0x8019a5d8): its frame-counter poll uses the same br2_irq_poll contract
 * as the spins here, and its timeout path sequences the three jals through
 * cd_jal. The
 * seqlock/frame-wait spins use the proven br2_irq_poll pattern: the VBlank
 * IRQ coroutine (cd_cb8_run_param -> DAT_801bdde0, the *0x801bcca8/
 * *0x801bccac counters) is driven by psx_check_interrupts exactly like
 * the generated per-block checks. The guest sp-0x28 frame is reserved via
 * br2_guest_frame so nested generated calls observe the same $sp. */
uint32_t cd_timer_read(int mode)
{
  uint32_t fr = br2_guest_frame(0x28);
  uint32_t p_cnt  = br2_ld32(0x801bcca8);
  uint32_t p_tick = br2_ld32(0x801bccac);
  uint32_t s0, s1, snap, cur, v0, a0v, a1v;
  (void)fr;

  s0 = br2_ld32(p_cnt);                       /* *(*0x801bcca8) */

  /* seqlock @0x8019A484: sample *p_tick until two reads agree */
  for (;;) {
    snap = br2_ld32(p_tick);
    cur  = br2_ld32(p_tick);
    if (snap == cur) break;
    if (br2_bail_pending()) { br2_guest_frame_free(); return 0; }
    br2_irq_poll();
  }

  s1 = (snap - br2_ld32(0x801bccb0)) & 0xffffu;

  if (mode < 0) {
    v0 = br2_ld32(0x801bdde0);
    br2_guest_frame_free();
    return v0;
  }
  if (mode == 1) {
    br2_guest_frame_free();
    return s1;
  }
  if (mode <= 0) {                          /* == 0 */
    v0  = br2_ld32(0x801bccb4);
    a1v = 0;
  } else {                                  /* >= 2 */
    v0  = br2_ld32(0x801bccb4) - 1u + (uint32_t)mode;
    a1v = (uint32_t)mode - 1u;
  }
  a0v = v0;
  call_guest4(0x8019a5d8u, a0v, a1v, 0, 0);   /* jal vsync_wait_n */

  s0 = br2_ld32(br2_ld32(0x801bcca8));         /* reload counter sample */
  call_guest4(0x8019a5d8u,
                     br2_ld32(0x801bdde0) + 1u, 1u, 0, 0); /* jal vsync_wait_n */

  /* @0x8019A53C: if bit 0x00400000 of the sample is set, wait for the
   * counter's sign bit (vs s0) to flip — @0x8019A54C/0x8019A56C loop. */
  if (s0 & 0x00400000u) {
    uint32_t pp = br2_ld32(0x801bcca8);
    for (;;) {
      cur = br2_ld32(pp);
      if (((s0 ^ cur) & 0x80000000u) != 0) break;
      if (br2_bail_pending()) { br2_guest_frame_free(); return 0; }
      br2_irq_poll();
    }
  }

  /* @0x8019A584: publish *0x801bdde0 into 0x801bccb4, then seqlock-copy
   * *(*0x801bccac) into 0x801bccb0 — @0x8019A59C loop. */
  v0     = br2_ld32(0x801bdde0);
  p_tick = br2_ld32(0x801bccac);
  br2_st32(0x801bccb4, v0);
  for (;;) {
    v0  = br2_ld32(p_tick);
    br2_st32(0x801bccb0, v0);
    cur = br2_ld32(0x801bccb0);
    v0  = br2_ld32(p_tick);
    if (cur == v0) break;
    if (br2_bail_pending()) { br2_guest_frame_free(); return 0; }
    br2_irq_poll();
  }

  br2_guest_frame_free();
  return s1;
}

/* ======== 0x8019a5d8 ======== */

/* vsync_wait_n — bounded VBlank-frame wait (0x8019a5d8, 152B/9blk/3jal,
 * decoded insn-by-insn against generated/SLES_017.22_full_46.c:25152).
 *
 * Guest semantics:
 *   sp -= 0x20; ctr = a1 << 15                 // @A5DC/A5E0 -> sp+0x10
 *   v0 = *0x801BDDE0 (VBlank frame counter, advanced by the cd_cb8 VBlank
 *        IRQ coroutine); if !((int32)v0 < a0) -> return  // @A5F0 slt/beq
 *   a3 = -1                                          // @A5FC
 *   loop @A600: ctr--; if ctr == -1 -> timeout @A620 // @A608-A618
 *               @A648: if !((int32)*0x801BDDE0 < a0) -> return
 *                      else loop                     // @A64C-A658
 *   timeout @A620: a0=0x80125B38 ("VSync timeout"); jal cd_str_print
 *                  ra=0x8019A62C; a0=0 (delay @A630); jal bios_table_tramp
 *                  B0/0x5b ra=0x8019A634; a0=3, a1=0 (delay @A63C); jal
 *                  bios_kobj_tramp C0/0x0a ra=0x8019A640; return.
 *
 * The frame-counter poll is the same IRQ-coroutine-driven flag class as
 * cd_timer_read's spins: br2_irq_poll supplies the per-block-boundary
 * psx_check_interrupts the generated CPS frame had, so the VBlank IRQ
 * advances *0x801BDDE0 while we spin. a1==0 makes ctr 0 -> first
 * decrement hits -1 -> the timeout/yield path runs immediately (the common
 * cd_timer_read(a0==0/1) shape). The three timeout jals go through
 * cd_jal so a0-a3 keep callee leftovers exactly like the guest (the BIOS
 * tramps forward live a1-a3). Guest frame reserved via br2_guest_frame so
 * nested guest calls see the same $sp; the ctr/ra spills are purely local
 * to this body so they stay C locals/regs. */
void vsync_wait_n(int frame_target,int timeout)
{
  uint32_t fr = br2_guest_frame(0x20);
  CPUState *cpu = clean_cpu_state();
  uint32_t counter = (uint32_t)timeout << 15;
  (void)fr;
  /* @A5F0: slt v0,*0x801BDDE0,a0 (signed); beq v0,zero -> epilogue */
  if (!((int32_t)br2_ld32(0x801BDDE0u) < frame_target)) {
    br2_guest_frame_free();
    return;
  }
  for (;;) {
    counter = counter - 1u;                        /* @A608-A60C */
    if (counter == 0xFFFFFFFFu) break;             /* @A618 -> @A620 */
    /* @A654: slt v0,*0x801BDDE0,a0; @A658 bne -> @A600, else return */
    if (!((int32_t)br2_ld32(0x801BDDE0u) < frame_target)) {
      br2_guest_frame_free();
      return;
    }
    if (br2_bail_pending()) { br2_guest_frame_abandon(); return; }
    br2_irq_poll();
  }
  /* timeout path @A620 — guest register state entering the first jal:
   * a0=0x80125B38, a1=timeout<<15 (prologue shift, untouched by loop),
   * a2=caller leftover (live in gpr[6] already), a3=-1 (@A5FC). */
  if (cpu) {
    cpu->gpr[5] = (uint32_t)timeout << 15;
    cpu->gpr[7] = 0xFFFFFFFFu;
    cpu->gpr[4] = 0x80125B38u;
    (void)cd_jal(0x80197AD0u, 0x8019A62Cu);          /* jal cd_str_print */
    if (br2_bail_pending()) { br2_guest_frame_abandon(); return; }
    cpu->gpr[4] = 0;                                /* delay slot @A630 */
    (void)cd_jal(0x80199FE0u, 0x8019A634u);          /* jal bios_table_tramp */
    if (br2_bail_pending()) { br2_guest_frame_abandon(); return; }
    cpu->gpr[4] = 3;                                /* @A634 */
    cpu->gpr[5] = 0;                                /* delay slot @A63C */
    (void)cd_jal(0x8019A670u, 0x8019A640u);          /* jal bios_kobj_tramp */
    if (br2_bail_pending()) { br2_guest_frame_abandon(); return; }
  }
  br2_guest_frame_free();
}

/* ======== 0x80195d44 ======== */

/* cd_result_wait_b_fwd — resolve guest-addr out_buf via guest_at, forward to cd_result_wait_b. */
void cd_result_wait_b_fwd(int a0,uint32_t out_buf)

{
  /* out_buf arrives as a raw guest address (uint32_t); the callee's
   * uint8_t* is a host pointer into guest memory — resolve it. */
  cd_result_wait_b(a0,out_buf ? (uint8_t *)guest_at(out_buf)
                                   : (uint8_t *)0);
  return;
}

/* ======== 0x801961cc ======== */

/* cd_busy_wait_fwd — forward to cd_busy_wait. */
void cd_busy_wait_fwd(int a0)

{
  cd_busy_wait(a0);
  return;
}

/* ======== 0x801961a8 ======== */

/* cd_cmd_dispatch3 — cd_dispatch_tbl4(3,a0): dispatch-table slot 3. */
int cd_cmd_dispatch3(uint32_t a0)

{
  return cd_dispatch_tbl4(3,a0);
}

/* ======== 0x80195d64 ======== */

/* mc_session — install cb as BIOS callback cell; returns previous. */
uint32_t mc_session(uint32_t cb)

{
  uint32_t prev_cb;

  prev_cb = CD_BIOS_CB_20;
  CD_BIOS_CB_20 = cb;
  return prev_cb;
}

/* ======== 0x80195d78 ======== */

/* mc_session_b — traced variant: CDFN + seq-ring record of cb/prev/ra; swaps CD_BIOS_CB_40. */
uint32_t mc_session_b(uint32_t cb)

{
  uint32_t prev_cb;

  CDFN(CDFN_MC_SESSION_B,cb);
  prev_cb = CD_BIOS_CB_40;
  CD_BIOS_CB_40 = cb;
  { uint32_t i = g_bc84c_seq_n & 63u;
    g_bc84c_seq[i*2]   = (uint32_t)cb;    /* new */
    g_bc84c_seq[i*2+1] = prev_cb;         /* old (pre-write bc84c) */
    g_bc84c_caller[i]  = clean_caller_ra();  /* guest $ra = calling fn */
    g_bc84c_seq_n++; }
  return prev_cb;
}

/* ======== 0x80195ec8 ======== */

/* cd_cmd_issue_async — async variant of cd_cmd_issue_sync: opcode zero-extended, retry loop w/ CD_BIOS_CB_20 handshake. */
uint32_t cd_cmd_issue_async(int8_t opcode,int param_addr)

{
  uint32_t saved_cb;
  int rc;
  int retries;
  /* guest zero-extends the opcode int8_t (andi $s4,$s1,0xff @0x80195EF0)
   * and uses that for the ==1 test, the 0x801bc7c0 table index and the
   * final cd_cmd_issue opcode — a signed int8_t would sign-extend into a
   * wild table index for opcodes >= 0x80. */
  uint32_t op = (uint32_t)(uint8_t)opcode;

  saved_cb = CD_BIOS_CB_20;
  retries = 3;
  while( true ) {
    CD_BIOS_CB_20 = 0;
    if ((op != 1) && ((CD_STATUS & 0x10) != 0)) {
      cd_cmd_issue(1,0,0,0);
    }
    if ((((param_addr == 0) || (I32AT(0x801bc7c0,op) == 0)) ||
        (rc = cd_cmd_issue(2,param_addr,0,0), rc == 0)) &&
       (CD_BIOS_CB_20 = saved_cb, rc = cd_cmd_issue((int8_t)op,param_addr,0,1), rc == 0)) break;
    retries = retries + -1;
    if (retries == -1) {
      CD_BIOS_CB_20 = saved_cb;
      return 0;
    }
  }
  return 1;
}

/* ======== 0x80198908 ======== */

/* cd_read_open — issues Pause/SetLoc/Setmode/ReadN for a read session.
 * Ported clean: the Setmode param int8_t lives on a real guest frame
 * (br2_guest_frame + live-$sp-derived buf addr) so generated
 * cd_cmd_issue sees the same buffer the guest's sp+0x10 would produce —
 * the earlier guest_sp()-0x18 version decoded to 0x1C instead of the
 * computed mode and wedged the read retry loop. */
uint32_t cd_read_open(int reopen)
{
  /* Faithful port of 0x80198908-0x80198AE4 (36 blocks, all leaves clean).
   * Guest frame is 0x28; the Setmode param int8_t lives at sp+0x10, i.e.
   * fr+0x10 on a real br2_guest_frame — the earlier guest_sp()-0x18
   * version resolved to a stale address and decoded mode 0x1C, wedging
   * the read retry. fr+0x10 is a live guest address so generated
   * cd_cmd_issue sees the same buffer the guest's sp+0x10 produces. */
  uint32_t fr = br2_guest_frame(0x28);
  uint8_t *setmode_buf;
  uint32_t mode_arg;
  int rc;
  uint32_t status, want_mode;

  mc_session(0);
  mc_session_b(0);
  if ((CD_DISPATCH_PEND & 1) != 0) {
    cd_cmd_dispatch3(0);
  }
  status = cd_status_raw();
  if ((status & 0x10) != 0) {
    /* status bit4 early-return */
    status = cd_timer_read(0xffffffff);
    if ((status & 0x3f) == 0) {
      cd_str_print(0x80125ae0);
    }
    cd_cmd_issue_async(1,0);
    CD_RD_TS_PAUSE = cd_timer_read(0xffffffff);
    CD_RD_RESULT = 0xffffffff;
    br2_guest_frame_free();
    return 0xffffffff;
  }
  if (reopen != 0) {
    cd_str_print(0x80125af8);
    cd_mode_flag_set(9,0,0);
    /* cd_mode_flag_set -> cd_cmd_issue treats param_addr as a guest address;
     * cd_param_buf_get returns a host pointer into guest RAM. */
    mode_arg = guest_of(cd_param_buf_get());
    rc = cd_mode_flag_set(2,(int)mode_arg,0);
    if (rc == 0) {
      /* mode_flag_set(2) fail */
      CD_RD_RESULT = 0xffffffff;
      br2_guest_frame_free();
      return 0xffffffff;
    }
  }
  cd_cmd_reset_wrap();
  /* stack buf = sp+0x10 (frame 0x28) -> fr+0x10; verified
   * `sb $s0,0x10($sp)` / `addiu $a1,$sp,0x10` in 0x80198908. */
  setmode_buf = (uint8_t *)guest_at(fr + 0x10);
  setmode_buf[0] = (uint8_t)CD_RD_MODE;
  want_mode = CD_RD_MODE & 0xff;
  status = cd_mode_byte_read();
  if (((want_mode != status) || (reopen != 0)) &&
      (rc = cd_mode_flag_set(0xe,(int)(fr + 0x10),0), rc == 0)) {
    /* mode_flag_set(0xe) fail */
    CD_RD_RESULT = 0xffffffff;
    br2_guest_frame_free();
    return 0xffffffff;
  }
  CD_RD_SECTOR = cd_stamp_to_sector(cd_param_buf_get());
  mc_session_b(0x801985c4);
  if ((CD_DISPATCH_PEND & 1) != 0) {
    cd_cmd_dispatch3(0x80198838);
  }
  CD_RD_CUR = CD_RD_POS;
  /* reached ReadN issue */
  /* sector count CD_RD_COUNT */
  cd_cmd_issue_async(6,0);
  CD_RD_RESULT = CD_RD_COUNT;
  CD_RD_TS_ISSUE = cd_timer_read(0xffffffff);
  br2_guest_frame_free();
  return CD_RD_RESULT;
}

/* ======== 0x80198590 ======== */

/* cd_init_done_cb — trace; mc_session(*0x801BCBA8); clear done flag @0x801BCBA4. */
void cd_init_done_cb(void)

{
  /* guest 0x80198590: a0 = lw(0x801BCBA8); jal 0x80195D64 (mc_session);
   * on return sw $zero,36($s0). $s0 = 0x801C0000-13400-40 = 0x801BCB80,
   * so the store clears the read-session completion flag @0x801BCBA4 —
   * NOT 0x801BFFFC (earlier mis-decode; that left bcba4=1 forever and
   * cd_read_poll reported busy -> infinite 37-sector read retry). */
  CDFN(CDFN_INIT_DONE_CB,0);
  mc_session(DB32(0x801bcba8));
  DB32(0x801bcba4) = 0;
}

/* ======== batch27: CD driver core ======== */

extern int  console_emit(int);
extern void console_pct(void);
extern int  cd_sync_wait_b(uint32_t,uint32_t);
extern int  cd_sync_wait_c(uint32_t,uint32_t);
extern void bios_table_tramp(void);
extern void bios_kobj_tramp(void);
extern uint16_t cd_u16_swap_store(uint16_t);
extern uint32_t gpu_status_cmd(uint32_t);

extern char *s_CD_sync_8012582c;
extern char *s_CD_ready_80125834;
extern char *s_CD_datasync_801258b0;
extern char *s_CD_timeout__801257a4;
extern char *s_VSync__timeout_80125b38;
extern char *s_CdRead__sector_error_80125ac8;
extern char *s_DiskError__801257d0;
extern char *s_com__s_code___02x__02x__801257dc;
extern char *s_CDROM__unknown_intr_801257f8;
extern char *s___d__8012580c;
extern char *s__s___s__Sync__s__Ready__s_801257b4;

#define CDB8(a)   DB8(a)
#define CDB8P(a)  DB8P(a)
#define CDB32(a)  DB32(a)
#define CDB32G(a) DB32G(a)
#define CDNAME(ch) (((uint32_t *)guest_at(0x801bc86c))[(ch) & 0xff])
#define CDNOINTR(st) (((uint32_t *)guest_at(0x801bc8ec))[(st) & 0xff])

/* shared sync-wait skeleton used by 968cc/96b4c/97698:
 * returns via out params; implemented inline per function for
 * faithfulness (they differ in exit conditions). */

/* ======== 0x801968cc ======== */

uint32_t cd_result_wait_a(int a0,uint8_t *out_buf)

{
  /* Delegate the whole body to generated MIPS (0x801968cc): the
   * Sync-wait while(true) loop must cross basic-block
   * boundaries each pass so psx_check_interrupts_at runs with
   * a live guest resume PC. A clean-C spin has no br2_irq_poll
   * and no per-block interrupt check, so cd_timer_read freezes
   * (the 0x3c0000 timeout never fires) AND the CD deferred-IRQ
   * coroutine never resumes to set 0x801bcb25/26 -> infinite
   * spin. Same contract as cd_init_session/cd_file_read_seq.
   * br2_call_generated makes gen_clean_table skip this address. */
  return (uint32_t)call_guest4(0x801968ccu, (uint32_t)a0,
                                  br2_gaddr(out_buf), 0, 0);
}

/* ======== 0x80197ad0 ======== */

/* cd_str_print — emit NUL-terminated guest string msg (default 0x801258C8) to console, then console_pct. */
void cd_str_print(uint32_t msg)

{
  char ch;
  uint8_t *s;

  s = msg ? (uint8_t *)guest_at(msg) : (uint8_t *)guest_at(0x801258c8);
  while( true ) {
    ch = (char)*s;
    s = s + 1;
    if (ch == '\0') break;
    console_emit((int)ch);
  }
  console_pct();
  return;
}

/* ======== 0x801972a8 ======== */

void cd_cmd_state_reset(void)

{
  /* Faithful port of generated 0x801972a8 (4 blocks). Issues reset params
   * through the channel pointer cells — *0x801bcb0c=1, *0x801bcb18=7,
   * *0x801bcb14=7 — then spins until the response channel drains:
   * (*(u8*)*0x801bcb18 & 7)==0. The drain is driven by the IRQ-side
   * response coroutine; br2_irq_poll() delivers pending IRQs against the
   * live CPUState exactly like the guest's per-block psx_check_interrupts,
   * the same mechanism the verified identical loop in cd_subsystem_init
   * (0x801974b8) already uses. Exit block pokes the 0x801bcb24 scratch
   * {2,0,0}, clears *0x801bcb0c and *0x801bcb18, and stores 0x1325 through
   * *0x801bcb1c — int8_t-for-int8_t the generated tail @0x8019732C-0x80197378. */
  uint8_t *q;
  br2_st8p(0x801bcb0c, 1);
  while ((br2_ld8p(0x801bcb18) & 7) != 0) {
    if (br2_bail_pending()) return;
    br2_irq_poll();
    br2_st8p(0x801bcb0c, 1);
    br2_st8p(0x801bcb18, 7);
    br2_st8p(0x801bcb14, 7);
  }
  q = (uint8_t *)guest_at(0x801bcb24);
  q[2] = 0;
  q[1] = q[2];
  q[0] = 2;
  br2_st8p(0x801bcb0c, 0);
  br2_st8p(0x801bcb18, 0);
  br2_st32p(0x801bcb1c, 0x1325);
  return;
}

/* ======== 0x8019a7d4 ======== */

uint16_t cd_intc_busy_read(void)

{
  /* guest 0x8019A7D4: lui $v0,0x801C; lh $v0,-0x32E6($v0); jr $ra; nop
   * -> signed halfword at guest 0x801C0000-0x32E6 = 0x801BCD1A (lh
   * sign-extends into $v0, but the uint16_t return truncates to the
   * same 16 bits). */
  return (uint16_t)br2_ld16(0x801bcd1a);
}

/* ======== 0x8019a87c ======== */

/* cd_irq_init_seq — CD IRQ/init sequencing coroutine (0x8019A87C-0x8019A8DB,
 * 8 CPS blocks). First clean-CPS port: proves the clean_cps.map resume
 * mechanism — every continuation/branch-target addr is registered so a yield
 * inside psx_check_interrupts_at re-dispatches back into THIS body with
 * cpu->pc == that addr, and the pc-switch below picks the block. All mutable
 * state lives in cpu->gpr[]/guest RAM (host locals die on the scheduler
 * longjmp). Each "jal" is a tail-dispatch: $ra = continuation, interrupt
 * check with the callee as resume_pc, cpu->pc = callee, return — the
 * generated CPS contract, preserved by the CLEAN_ENT_CPS flag in
 * clean_try_dispatch (which skips the forced cpu->pc = $ra epilogue).
 *
 * Block map (verified vs generated/SLES_017.22_full_46.c):
 *   87C entry : v0==0 -> 88C ; v0!=0 -> 884 (call cd_intc_dispatch first)
 *   884       : ra=88C, jal cd_intc_dispatch
 *   88C       : s0=0x801C0000-12972, a0=s0-4, v0=s0+4060, sw v0,0(s0),
 *               ra=8A4, jal bios_tramp_b019
 *   8A4       : v0=1, sh v0,-60(s0), ra=8B0, jal cd_init_stage_a
 *   8B0       : v1=*stash_ptr, sw v0,20(v1), ra=8C0, jal cd_init_stage_b
 *   8C0       : a0=*stash_ptr, sw v0,4(a0), ra=8D0, jal bios_tramp_a072
 *   8D0       : ra=8D8, s0-=60, jal cpu_critical_exit
 *   8D8       : v0=s0, fallthrough -> next fn (stays generated)
 * The sw/sh stores write the INTC/init results into the CD context block —
 * omitting them starves pending CD commands (the wedge this port fixes).
 */
uint32_t cd_irq_init_seq(void)
{
  extern void psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);
  CPUState *c = clean_cpu_state();
  uint32_t pc;
  if (!c) return 0;
  pc = c->pc;
  if (pc != 0u && pc != 0x8019A87Cu) {
    switch (pc) {
    case 0x8019A884u: goto b884;
    case 0x8019A88Cu: goto b88C;
    case 0x8019A8A4u: goto b8A4;
    case 0x8019A8B0u: goto b8B0;
    case 0x8019A8C0u: goto b8C0;
    case 0x8019A8D0u: goto b8D0;
    case 0x8019A8D8u: goto b8D8;
    default: break;
    }
  }
  /* block_8019A87C — entry: branch on (v0 == 0) */
  if (c->gpr[2] == c->gpr[0]) {
    psx_check_interrupts_at(c, 0x8019A88Cu);
    goto b88C;
  }
  psx_check_interrupts_at(c, 0x8019A884u);
  goto b884;

b884:
  c->gpr[31] = 0x8019A88Cu;
  psx_check_interrupts_at(c, 0x8019A8ECu);
  c->pc = 0x8019A8ECu;
  return c->gpr[2];

b88C:
  c->gpr[16] = 0x801C0000u;
  c->gpr[16] = c->gpr[16] - 12972u;          /* s0 = ctx base */
  c->gpr[4]  = c->gpr[16] - 4u;              /* a0  = s0-4 */
  c->gpr[2]  = c->gpr[16] + 4060u;           /* v0  = s0+4060 */
  br2_st32(c->gpr[16], c->gpr[2]);           /* sw v0,0(s0) */
  c->gpr[31] = 0x8019A8A4u;
  psx_check_interrupts_at(c, 0x8019AD80u);
  c->pc = 0x8019AD80u;
  return c->gpr[2];

b8A4:
  c->gpr[2] = 1u;
  br2_st16(c->gpr[16] - 60u, (uint16_t)c->gpr[2]); /* sh v0,-60(s0) */
  c->gpr[31] = 0x8019A8B0u;
  psx_check_interrupts_at(c, 0x8019AE10u);
  c->pc = 0x8019AE10u;
  return c->gpr[2];

b8B0:
  c->gpr[3] = 0x801C0000u;
  c->gpr[3] = br2_ld32(c->gpr[3] - 8800u);   /* v1 = stash ptr */
  br2_st32(c->gpr[3] + 20u, c->gpr[2]);      /* sw v0,20(v1) */
  c->gpr[31] = 0x8019A8C0u;
  psx_check_interrupts_at(c, 0x8019AF30u);
  c->pc = 0x8019AF30u;
  return c->gpr[2];

b8C0:
  c->gpr[4] = 0x801C0000u;
  c->gpr[4] = br2_ld32(c->gpr[4] - 8800u);   /* a0 = stash ptr */
  br2_st32(c->gpr[4] + 4u, c->gpr[2]);       /* sw v0,4(a0) */
  c->gpr[31] = 0x8019A8D0u;
  psx_check_interrupts_at(c, 0x8019AD48u);
  c->pc = 0x8019AD48u;
  return c->gpr[2];

b8D0:
  c->gpr[31] = 0x8019A8D8u;
  c->gpr[16] = c->gpr[16] - 60u;            /* delay-slot: s0 -= 60 */
  psx_check_interrupts_at(c, 0x8018E310u);
  c->pc = 0x8018E310u;
  return c->gpr[2];

b8D8:
  c->gpr[2] = c->gpr[16];                    /* v0 = s0 */
  c->pc = 0x8019A8DCu;                       /* fallthrough -> next fn */
  return c->gpr[2];
}

/* ======== 0x801979F8 ======== */

/* cd_response_pump — CD deferred-work pump loop (0x801979F8-0x80197ACB,
 * 12 CPS blocks). The hot coroutine that drives cd_response_process
 * (0x80196370, already clean-ported): each iteration tail-calls it, reads
 * the returned status in s0, and dispatches on status bits —
 *   s0==0        -> exit (commit state int8_t, restore frame, jr ra)
 *   s0&4 & *handlerA!=0 -> jalr handlerA(stateA, 0x801FC6D0) -> A6C
 *   s0&2 & *handlerB!=0 -> jalr handlerB(stateB, 0x801FC6D0) -> A9C -> loop
 *   else         -> loop back to A2C.
 * Prologue builds a 0x28 guest frame (sp-=40, saves s0-s3+ra); the frame
 * lives in guest RAM so it survives scheduler longjmps — all mutable state
 * stays in cpu->gpr[]/guest memory, never host locals. The two jalr targets
 * are runtime fn-ptrs tail-dispatched (cpu->pc=target) like generated CPS
 * jalr, returning to the gpr[31] continuation block.
 * clean_cps.map registers entry + all 11 continuation addrs. */
uint32_t cd_response_pump(void)
{
  extern void psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);
  CPUState *c = clean_cpu_state();
  uint32_t pc;
  if (!c) return 0;
  pc = c->pc;
  if (pc != 0u && pc != 0x801979F8u) {
    switch (pc) {
    case 0x80197A2Cu: goto bA2C;
    case 0x80197A34u: goto bA34;
    case 0x80197A40u: goto bA40;
    case 0x80197A48u: goto bA48;
    case 0x80197A5Cu: goto bA5C;
    case 0x80197A6Cu: goto bA6C;
    case 0x80197A70u: goto bA70;
    case 0x80197A78u: goto bA78;
    case 0x80197A8Cu: goto bA8C;
    case 0x80197A9Cu: goto bA9C;
    case 0x80197AA4u: goto bAA4;
    default: break;
    }
  }

  /* block_801979F8 — entry: build frame, load state int8_t */
  c->gpr[2]  = 0x801C0000u;
  c->gpr[2]  = br2_ld32(c->gpr[2] - 13556u);   /* v0 = *(u32*)state-cell = ptr */
  c->gpr[29] = c->gpr[29] - 40u;               /* sp -= 0x28 */
  br2_st32(c->gpr[29] + 20u, c->gpr[17]);      /* save s1 */
  c->gpr[17] = 0x801C0000u;
  c->gpr[17] = c->gpr[17] - 13531u;            /* s1 = state-int8_t table base */
  br2_st32(c->gpr[29] + 32u, c->gpr[31]);      /* save ra */
  br2_st32(c->gpr[29] + 28u, c->gpr[19]);      /* save s3 */
  br2_st32(c->gpr[29] + 24u, c->gpr[18]);      /* save s2 */
  br2_st32(c->gpr[29] + 16u, c->gpr[16]);      /* save s0 */
  c->gpr[2]  = br2_ld8(c->gpr[2]);             /* v0 = *(u8*)ptr = state int8_t */
  c->gpr[19] = c->gpr[17] - 1u;                /* s3 = s1-1 */
  c->gpr[18] = c->gpr[2] & 3u;                 /* s2 = state & 3 */
  /* fall through */

bA2C:
  c->gpr[31] = 0x80197A34u;                    /* jal link */
  psx_check_interrupts_at(c, 0x80196370u);
  c->pc = 0x80196370u;                         /* tail-call cd_response_process */
  return c->gpr[2];

bA34:
  c->gpr[16] = c->gpr[2];                      /* s0 = status (v0) */
  { int _bc = (c->gpr[16] == c->gpr[0]);       /* s0 == 0 ? */
    c->gpr[2] = c->gpr[16] & 4u;               /* delay: v0 = s0&4 */
    if (_bc) { psx_check_interrupts_at(c, 0x80197AA4u); goto bAA4; }
    psx_check_interrupts_at(c, 0x80197A40u); goto bA40; }

bA40:
  { int _bc = (c->gpr[2] == c->gpr[0]);        /* (s0&4) == 0 ? */
    c->gpr[2] = c->gpr[16] & 2u;               /* delay: v0 = s0&2 */
    if (_bc) { psx_check_interrupts_at(c, 0x80197A70u); goto bA70; }
    psx_check_interrupts_at(c, 0x80197A48u); goto bA48; }

bA48:
  c->gpr[2] = 0x801C0000u;
  c->gpr[2] = br2_ld32(c->gpr[2] - 14260u);    /* v0 = *(u32*)handlerA cell */
  { int _bc = (c->gpr[2] == c->gpr[0]);        /* handlerA == NULL ? */
    if (_bc) { psx_check_interrupts_at(c, 0x80197A6Cu); goto bA6C; }
    psx_check_interrupts_at(c, 0x80197A5Cu); goto bA5C; }

bA5C:
  c->gpr[4] = br2_ld8(c->gpr[17]);             /* a0 = *(u8*)s1 = state int8_t A */
  c->gpr[5] = 0x80200000u;                     /* a1 base (delay slot) */
  c->gpr[5] = c->gpr[5] - 14640u;              /* a1 = 0x801FC6D0 */
  c->gpr[31] = 0x80197A6Cu;                    /* jalr link */
  { uint32_t _jt = c->gpr[2];                  /* handlerA fn-ptr */
    psx_check_interrupts_at(c, _jt);
    c->pc = _jt; return c->gpr[2]; }           /* jalr -> handlerA */

bA6C:
  c->gpr[2] = c->gpr[16] & 2u;                 /* v0 = s0&2 */
  /* fall through */

bA70:
  { int _bc = (c->gpr[2] == c->gpr[0]);        /* (s0&2) == 0 ? */
    if (_bc) { psx_check_interrupts_at(c, 0x80197A2Cu); goto bA2C; }  /* loop */
    psx_check_interrupts_at(c, 0x80197A78u); goto bA78; }

bA78:
  c->gpr[2] = 0x801C0000u;
  c->gpr[2] = br2_ld32(c->gpr[2] - 14264u);    /* v0 = *(u32*)handlerB cell */
  { int _bc = (c->gpr[2] == c->gpr[0]);        /* handlerB == NULL ? */
    if (_bc) { psx_check_interrupts_at(c, 0x80197A2Cu); goto bA2C; }  /* loop */
    psx_check_interrupts_at(c, 0x80197A8Cu); goto bA8C; }

bA8C:
  c->gpr[4] = br2_ld8(c->gpr[19]);             /* a0 = *(u8*)s3 = state int8_t B */
  c->gpr[5] = 0x80200000u;                     /* a1 base (delay slot) */
  c->gpr[5] = c->gpr[5] - 14640u;              /* a1 = 0x801FC6D0 */
  c->gpr[31] = 0x80197A9Cu;                    /* jalr link */
  { uint32_t _jt = c->gpr[2];                  /* handlerB fn-ptr */
    psx_check_interrupts_at(c, _jt);
    c->pc = _jt; return c->gpr[2]; }           /* jalr -> handlerB */

bA9C:
  psx_check_interrupts_at(c, 0x80197A2Cu);
  goto bA2C;                                   /* loop */

bAA4:
  /* s0 == 0 — exit: commit state int8_t, restore frame, jr ra */
  c->gpr[2] = 0x801C0000u;
  c->gpr[2] = br2_ld32(c->gpr[2] - 13556u);    /* v0 = state ptr (reload) */
  br2_st8(c->gpr[2], (uint8_t)c->gpr[18]);     /* sb s2,0(ptr) — commit state */
  c->gpr[31] = br2_ld32(c->gpr[29] + 32u);     /* restore ra */
  c->gpr[19] = br2_ld32(c->gpr[29] + 28u);     /* restore s3 */
  c->gpr[18] = br2_ld32(c->gpr[29] + 24u);     /* restore s2 */
  c->gpr[17] = br2_ld32(c->gpr[29] + 20u);     /* restore s1 */
  c->gpr[16] = br2_ld32(c->gpr[29] + 16u);     /* restore s0 */
  c->gpr[29] = c->gpr[29] + 40u;               /* sp += 0x28 */
  { uint32_t _jt = c->gpr[31];                 /* jr ra */
    psx_check_interrupts_at(c, _jt);
    c->pc = _jt; return c->gpr[2]; }
}

/* ======== 0x80196370 ======== */

/* cd_response_process — CD response-int8_t state machine (0x80196370-
 * 0x801968C8, 343 insns decoded insn-by-insn from disc/SLES_017.22).
 * IRQ2-context leaf called by cd_response_process's only caller
 * cd_response_pump (and inlined into cd_result_wait_b's generated twin).
 *
 * Wait classification (the §4 test): the ONLY unbounded wait is the
 * status-stabilize loop @0x801963C4-0x801963E4, which polls
 * lbu[*0x801bcb18]&7 until two consecutive reads agree — a CD-hardware
 * status int8_t advanced by the CD device model / INT1 coroutine, i.e.
 * IRQ-handler-updated, NOT a separate cooperative thread. Same class as
 * the proven cd_cmd_state_reset drain loop (0x801972a8) and
 * vsync_wait_n. The 8-int8_t response read @0x801963EC is bounded (s0<8)
 * and gated on (*0x801bcb0c)&0x20. Everything else is straight-line
 * state-machine work + rare printf paths. PORTABLE.
 *
 * Guest layout: frame 0x30; locals sp+0x10=status&7, sp+0x18..0x1F =
 * 8-int8_t response buffer (private — IRQ handlers never read caller
 * frames, so a host buffer is faithful). Pointer cells are re-loaded
 * every access exactly like the guest's lw-ptr; pointee accesses go
 * through br2_ld8p/br2_st8p/br2_st32p so MMIO pointees route correctly.
 * s0 = response-int8_t count, s1 = buf[0]&0x1D reply-class flags.
 *
 * Jump table @0x80125818 (status-1 -> case): verified against the disc:
 *   [0] status1 -> 0x80196718  ret 4  (CB25 = s1?5:1 after s0==1 clear,
 *                                 copy->C6D0, *CB0C-ptr=0, *CB18-ptr=0)
 *   [1] status2 -> 0x801966CC  ret 2  (CB24 = s1?5:2, copy->C6C8)
 *   [2] status3 -> 0x801965CC  ret 1/2 (s1!=0: CB24=5 ret2;
 *                                 s1==0: tblB[0x801bc924+idx*4]?CB24=3
 *                                 ret1 : CB24=2 ret2; copy->C6C8)
 *   [3] status4 -> 0x80196798  ret 4  (CB26=4,CB25=4, copy->C6D8,
 *                                 copy->C6D0)
 *   [4] status5 -> 0x80196818  ret 6  (CB25=5,CB24=5, copy->C6C8,
 *                                 copy->C6D0)
 *   default    -> 0x80196898  ret 0  (cd_str_print+printf, status>=6)
 * The dead `beq $v1,$zr` guards before each copy test a constant
 * 0x8020C6Cx (never zero) — the guest always copies; reproduced as
 * unconditional 8-int8_t copies. */
/* cd_response_process — CD response-int8_t state machine. DELEGATED to
 * generated (0x80196370): the clean port wedged attract/boot at ~frame
 * 15k with pending-cmd-0x0A starvation — the response bytes are produced
 * by the deferred-coroutine chain (cd_ctx_restore->cd_chan_full_init->
 * cd_intc_dispatch) that needs the generated CPS live-resume-PC, which
 * clean C cannot supply. Per AGENTS.md §4 this stays generated-
 * authoritative until the coroutine runtime exists. */
uint32_t cd_response_process(void)
{
  return (uint32_t)call_guest4(0x80196370u,0,0,0,0);
}

/* ======== small getters ======== */

/* cd_status_raw — raw CD status byte @0x801bc854. */
uint8_t cd_status_raw(void)

{
  return (uint8_t)CD_STATUS;
}

/* cd_mode_byte_read — CD mode/state byte @0x801bc864. */
uint8_t cd_mode_byte_read(void)

{
  return DB8(0x801bc864);
}

/* cd_param_buf_get — pointer to the CD parameter buffer cell @0x801bc860. */
uint8_t * cd_param_buf_get(void)

{
  return (uint8_t *)guest_at(0x801bc860);
}

/* cd_cmd_reset_wrap — thin wrapper over cd_cmd_state_reset. */
void cd_cmd_reset_wrap(void)

{
  cd_cmd_state_reset();
  return;
}

/* ======== 0x80195ffc ======== */

bool cd_cmd_issue_sync(int8_t opcode,int param_addr,uint32_t param_val)

{
  uint32_t saved_cb;
  bool ok;
  int rc;
  int issued;
  int retries;
  int wait_rc;
  /* guest zero-extends the opcode int8_t (andi $s3,$s4,0xff @0x8019602C)
   * for the ==1 test, the 0x801bc7c0 table index and the final
   * cd_cmd_issue opcode — see cd_cmd_issue_async. */
  uint32_t op = (uint32_t)(uint8_t)opcode;

  saved_cb = CD_BIOS_CB_20;
  retries = 3;
  do {
    CD_BIOS_CB_20 = 0;
    if ((op != 1) && ((CD_STATUS & 0x10) != 0)) {
      cd_cmd_issue(1,0,0,0);
    }
    if (((param_addr == 0) || (I32AT(0x801bc7c0,op) == 0)) ||
       (rc = cd_cmd_issue(2,param_addr,param_val,0), rc == 0)) {
      CD_BIOS_CB_20 = saved_cb;
      rc = cd_cmd_issue((int8_t)op,param_addr,param_val,0);
      issued = 0;
      if (rc == 0) break;
      /* cd_cmd_issue(opcode) returned nonzero */
    }
    retries = retries + -1;
    issued = -1;
    CD_BIOS_CB_20 = saved_cb;
  } while (retries != -1);
  if (issued == 0) {
    /* param_val is a raw guest address value; cd_result_wait_a's
     * uint8_t* param is a host pointer — resolve via guest_at. */
    wait_rc = cd_result_wait_a(0,param_val ? (uint8_t *)guest_at(param_val)
                                      : (uint8_t *)0);
    ok = wait_rc == 2;
  }
  else {
    ok = false;
  }
  return ok;
}

/* ======== 0x801962f0 ======== */

/* cd_stamp_to_sector — decode 3-byte BCD-ish timestamp stamp to sector number (-0x96 bias). */
int cd_stamp_to_sector(const void *stamp)

{
  const uint8_t *s = (const uint8_t *)stamp;
  return (((uint32_t)(s[0] >> 4) * 10 + (s[0] & 0xf)) * 0x3c +
         (uint32_t)(s[1] >> 4) * 10 + (s[1] & 0xf)) * 0x4b +
         (uint32_t)(s[2] >> 4) * 10 + (s[2] & 0xf) + -0x96;
}

/* ======== 0x80196b4c ======== */

uint32_t cd_result_wait_b(int a0,uint8_t *out_buf)

{
  /* Delegate the whole body to generated MIPS (0x80196b4c): the
   * Sync-wait while(true) loop must cross basic-block
   * boundaries each pass so psx_check_interrupts_at runs with
   * a live guest resume PC. A clean-C spin has no br2_irq_poll
   * and no per-block interrupt check, so cd_timer_read freezes
   * (the 0x3c0000 timeout never fires) AND the CD deferred-IRQ
   * coroutine never resumes to set 0x801bcb25/26 -> infinite
   * spin. Same contract as cd_init_session/cd_file_read_seq.
   * br2_call_generated makes gen_clean_table skip this address. */
  return (uint32_t)call_guest4(0x80196b4cu, (uint32_t)a0,
                                  br2_gaddr(out_buf), 0, 0);
}

/* ======== 0x80197698 ======== */

/* cd_busy_wait — CD busy/timeout poll (0x80197698, 14 blocks verified).
 * Arms a deadline at *0x801fc6e0 = cd_timer_read(-1) + 960, zeroes the
 * iteration counter *0x801fc6e4 and plants 0x801258b0 at *0x801fc6e8,
 * then loops: cd_timer_read(-1) -> bail to the timeout path when the
 * deadline passes (signed slt) or the counter exceeds 0x003c0000;
 * otherwise poll the CD status channel **(u32*)*0x801bcb50 & 0x01000000
 * (device-deadline bit — br2_irq_poll advances guest cycles so the CD
 * device model clears it, same contract as cd_cmd_issue_b's reg50 wait).
 * Bit clear -> return 0; a0 != 0 -> return 1; a0 == 0 -> loop.
 * Timeout path: cd_str_print("CD timeout"), then printf_forward_guest
 * (0x8018e550) with fmt 0x801257b4, a1=*0x801fc6e8, a2=*(*0x801bc86c +
 * lbu(0x801bc865)*4), a3=*(*0x801bc8ec + lbu(0x801bcb24)*4), stack arg
 * *( *0x801bc8ec + lbu(0x801bcb25)*4 ) at sp+0x10 — reproduced through a
 * real 0x30 guest frame + call_guest4 so the vararg wrapper reads the
 * same stack slots — then cd_cmd_state_reset() and return -1.
 * cd_timer_read is now clean (8663946); cd_str_print/cd_cmd_state_reset
 * were already clean; only the printf jal needed call_guest4. */
uint32_t cd_busy_wait(int report_busy)
{
  uint32_t fr = br2_guest_frame(0x30);
  const uint32_t s0 = 0x801bc8ec, s1 = 0x801bcb24, s3 = 0x801bc86c;
  const uint32_t s4 = 0x003c0000;
  uint32_t s2 = (uint32_t)report_busy;
  uint32_t v0, cnt, busy_bit, b1, b2, b3, a2v, a3v;

  v0 = cd_timer_read(-1) + 960u;
  br2_st32(0x801fc6e0, v0);
  br2_st32(0x801fc6e4, 0);
  br2_st32(0x801fc6e8, 0x801258b0);

  for (;;) {
    v0 = cd_timer_read(-1);
    if ((int32_t)br2_ld32(0x801fc6e0) < (int32_t)v0) goto timeout;
    cnt = br2_ld32(0x801fc6e4);
    br2_st32(0x801fc6e4, cnt + 1u);            /* delay-slot store */
    if ((int32_t)s4 < (int32_t)cnt) goto timeout;
    busy_bit = br2_ld32p(0x801bcb50) & 0x01000000u;
    if (busy_bit == 0) { br2_guest_frame_free(); return 0; }
    if (s2 != 0)       { br2_guest_frame_free(); return 1; }
    /* report_busy == 0: keep waiting for the busy bit to clear */
  }

timeout:
  cd_str_print(0x801257a4);
  b1  = br2_ld8(s1);                            /* lbu 0x801bcb24 */
  b2  = br2_ld8(s1 + 1);                        /* lbu 0x801bcb25 */
  v0  = br2_ld32(s0 + (b2 << 2));               /* stack arg -> sp+0x10 */
  b3  = br2_ld8(0x801bc865);
  a2v = br2_ld32(s3 + (b3 << 2));
  a3v = br2_ld32(s0 + (b1 << 2));
  /* mirror the guest frame stores the printf vararg wrapper reads:
   * sp+0x18=s0, sp+0x1c=s1, sp+0x20=s2(param), sp+0x24=s3, sp+0x28=s4. */
  br2_st32(fr + 0x10, v0);
  br2_st32(fr + 0x18, s0);
  br2_st32(fr + 0x1c, s1);
  br2_st32(fr + 0x20, s2);
  br2_st32(fr + 0x24, s3);
  br2_st32(fr + 0x28, s4);
  (void)call_guest4(0x8018e550, 0x801257b4, br2_ld32(0x801fc6e8), a2v, a3v);
  cd_cmd_state_reset();
  br2_guest_frame_free();
  return (uint32_t)0xffffffffu;
}

/* ======== 0x8019a6e0 ======== */

int cd_dispatch_tbl4(int a0,uint32_t a1)

{
  /* Faithful port of generated 0x8019a6e0 (CPS chain verified): pure
   * indirect dispatch — tbl = *0x801BDDA0, fn = *(tbl+4), jalr fn with
   * the caller's a0/a1 (a2/a3 are caller leftovers; call_guest zeroes
   * them, identical to the old (p1,p2,0,0) thunk). No waits, no MMIO,
   * no deferred-coroutine state — the IRQ-boundary comment was stale. */
  uint32_t tbl = CDB32G(0x801bdda0);
  uint32_t fn = br2_ld32(tbl + 4);
  return (int)call_guest(fn,(uint32_t)a0,a1);
}

/* ======== 0x801985c4 ======== */

/* cd_read_step_cb — per-sector read-step callback (0x801985C4-0x80198834,
 * 0x274 bytes decoded insn-by-insn). Session struct S = 0x801CCB80:
 *   +0x10 mode (==0x200 gate), +0x14 sector count (int, -1 = error/reopen),
 *   +0x18 last timer snapshot, +0x1C deadline base (+0x4B0 = deadline),
 *   +0x20 expected sector (= *0x801CCBA0), +0x24 completion flag,
 *   +0x2C mc_session_b arg, +0x30 cd_cmd_dispatch3 arg, +0x34 a1.
 * Wait classification: NO wait of its own — every blocking call
 * (cd_sync_wait_b/c -> cd_cmd_issue_b/c, cd_busy_wait, cd_timer_read,
 * cd_read_open, cd_cmd_issue_async) is already clean and polls
 * IRQ/hardware-updated flags via br2_irq_poll. Event-driven step ->
 * PORTABLE. The sp+0x10 stamp buffer is a real guest frame (reg48 gets
 * the guest address; the CD response writes the stamp there and
 * cd_stamp_to_sector reads it back). */
/* cd_read_step_cb — per-sector read-step callback (0x801985C4).
 * DELEGATED to generated: part of the 16c4e96 CD-port batch that wedged
 * attract/boot. The mode-0x200 command sequencing + session teardown +
 * user-callback jalr need generated CPS state; stays generated-
 * authoritative per AGENTS.md §4. */
void cd_read_step_cb(uint32_t a0,uint32_t a1)
{
  call_guest4(0x801985c4u,a0,a1,0,0);
}

/* ======== 0x80198838 ======== */

/* cd_read_finish_cb — read-session completion callback: advances the sector
 * cursor (bcb88 += bcb90*4), decrements sectors-remaining bcb94, and on the
 * final sector issues the Pause(9) + fires the registered user callback
 * (0x801bc7b0 -> DeliverEvent). This is the same IRQ/completion-path family
 * as cd_read_step_cb: it calls cd_cmd_issue_async and call_guest across the
 * deferred-IRQ coroutine boundary, and a clean-C body cannot supply the live
 * guest register file / frame the generated MIPS keeps. CD_RD_DONE stayed 0
 * at the stall (completion branch never ran) — delegate the whole body to the
 * recompiled 0x80198838 so the Pause + DeliverEvent fire faithfully. */
uint32_t cd_read_finish_cb(void)
{
  /* Faithful port of generated 0x80198838 (decoded insn-by-insn):
   *   s0 = 0x801BCB80 (lui/addiu — a fixed session struct, args unused)
   *   *(s0+8)  |= *(s0+0x10) << 2        // bcb88 += bcb90*4
   *   --*(s0+0x14); ++*(s0+0x20)         // bcb94--, bcba0++
   *   if (*(s0+0x14) != 0) return 1-ish  // bne -> epilogue, v0 untouched-ish
   *   mc_session_b(*(s0+0x2C))
   *   if (*(0x801BC7B4) & 1) cd_cmd_dispatch3(*(s0+0x30))
   *   mc_session(0x80198590)             // = &cd_init_done_cb
   *   cd_cmd_issue_async(9, 0)           // Pause
   *   *(s0+0x24) = 1                     // bcba4 completion flag (delay slot
   *                                      // of the cb-null beq — always runs)
   *   cb = *(0x801BC7B0); if (cb) return call_guest(cb,2,*(s0+0x34));
   *   return 1;
   * All calls are ordinary guest `jal`/`jalr` — reproduced via call_guest,
   * which supplies the nested-dispatch contract (caller $ra stop, a2/a3
   * leftovers preserved). For the 1-arg calls guest leaves $a1 as a caller
   * leftover, so the live gpr[5] is passed back unchanged. The dynamic
   * callback's $ra token is the epilogue itself — reproduced by returning
   * the call_guest result straight to the wrapper. No loops, no BIOS/kernel
   * boundary inside this body, no gpr-file snapshot. */
  uint32_t s0 = 0x801bcb80;
  uint32_t cb, r, cnt;
  CDFN(CDFN_READ_FINISH_CB,0);
  DB32(s0 + 0x08) = DB32(s0 + 0x08) | (DB32(s0 + 0x10) << 2);
  DB32(s0 + 0x14) = DB32(s0 + 0x14) - 1;
  DB32(s0 + 0x20) = DB32(s0 + 0x20) + 1;
  cnt = DB32(s0 + 0x14);               /* lw $v0,+0x14 reload — bne $v0 */
  if (cnt != 0) return cnt;
  (void)call_guest(0x80195d78, DB32(s0 + 0x2c), clean_cpu_state()->gpr[5]);
  if ((DB32(0x801bc7b4) & 1) != 0)
    (void)call_guest(0x801961a8, DB32(s0 + 0x30), clean_cpu_state()->gpr[5]);
  (void)call_guest(0x80195d64, 0x80198590, clean_cpu_state()->gpr[5]);
  (void)call_guest(0x80195ec8, 9, 0);
  DB32(s0 + 0x24) = 1;
  cb = DB32(0x801bc7b0);
  if (cb != 0) {
    r = call_guest(cb, 2, DB32(s0 + 0x34));
    return r;
  }
  return 1;
}

/* ---- batch28: CD sync-wait wrappers + BIOS trampolines ---- */
extern uint32_t cd_cmd_issue_b(uint32_t,uint32_t);
extern uint32_t cd_cmd_issue_c(uint32_t,uint32_t);
extern void cd_cmd_bytes(uintptr_t);

uint32_t cd_sync_wait_a(int a0,uint8_t *out_buf)
{
  return cd_result_wait_a(a0,out_buf);
}

/* cd_sync_wait_b — sync-wait variant B: cd_cmd_issue_b, returns 1 on issue success (ret 0). */
int cd_sync_wait_b(uint32_t a0,uint32_t a1)
{
  return cd_cmd_issue_b(a0,a1) == 0;
}

/* cd_sync_wait_c — sync-wait variant C: cd_cmd_issue_c, returns 1 on issue success (ret 0). */
int cd_sync_wait_c(uint32_t a0,uint32_t a1)
{
  return cd_cmd_issue_c(a0,a1) == 0;
}

/* BIOS table trampolines — guest stubs are `li t2,<vec>; jr t2;
 * li t1,<svc>` forwarding caller a0-a3. Disasm-verified:
 * 0x80199fe0 = B0/0x5b, 0x8019a670 = C0/0x0a, 0x801957f0 = A0/0x44. */
extern void br2_bios_tramp(uint32_t,uint32_t);
extern uint32_t br2_call_generated(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
void bios_table_tramp(void)
{
  br2_bios_tramp(0xb0,0x5b);
}

/* bios_kobj_tramp — BIOS C0/0x0a kernel-object trampoline (guest 0x8019a670); see bios_table_tramp note above. */
void bios_kobj_tramp(void)
{
  br2_bios_tramp(0xc0,0x0a);
}

/* ---- batch29: CD command submit + status ---- */

/* cd_cmd_issue_b — CD command issue A (a0/a1 -> regs 48/4c),
 * polls status bit 0x40 then waits DMA-clear on reg 0x50.
 *
 * Faithful port of generated 0x80197800 (6 blocks, no jal/BIOS/fn-ptr).
 * Every 0x801bcbXX access is a pointer-cell indirection — br2_st*p /
 * br2_ld*p reproduce the guest's lw-ptr; sb/sw *ptr exactly (this is the
 * mistranslation that caused the earlier boot file-load livelock; the
 * pointer-cell helpers are the fix, not a reason to delegate). Sequence:
 * status-cmd bytes {0,0x80}, reg40=0x00020943, reply=0x1323,
 * reg44|=0x8000, reg48=a0, reg4c=a1|0x10000, then spin while
 * (*(*0x801bcb0c)&0x40)==0 (done bit, IRQ/response-coroutine delivered —
 * br2_irq_poll supplies the per-block interrupt check the generated CPS
 * frames had). Tail: reg50=0x11000000, spin while the latched reg's
 * 0x01000000 bit stays set (device-cycle deadline — br2_irq_poll advances
 * guest cycles so the CD device model clears it), reply 0x1323, ret 0. */
uint32_t cd_cmd_issue_b(uint32_t arg48,uint32_t arg4c)
{
  uint32_t reg;
  br2_st8p(0x801bcb0c, 0);
  br2_st8p(0x801bcb18, 0x80);
  br2_st32p(0x801bcb40, 0x00020943);
  br2_st32p(0x801bcb1c, 0x1323);
  br2_st32p(0x801bcb44, br2_ld32p(0x801bcb44) | 0x8000);
  br2_st32p(0x801bcb48, arg48);
  br2_st32p(0x801bcb4c, arg4c | 0x10000);
  while ((br2_ld8p(0x801bcb0c) & 0x40) == 0) {
    if (br2_bail_pending()) return 0;
    br2_irq_poll();
  }
  br2_st32p(0x801bcb50, 0x11000000);
  reg = br2_ld32p(0x801bcb50);          /* guest latches $a0 = *0x801bcb50 */
  while ((br2_ld32(reg) & 0x01000000) != 0) {
    if (br2_bail_pending()) return 0;
    br2_irq_poll();
  }
  br2_st32p(0x801bcb1c, 0x1323);
  return 0;
}

/* cd_cmd_issue_c — CD command issue B (same protocol, different opcode
 * 0x21020843 / reply 0x11400100). Faithful port of generated 0x80197900
 * (3 blocks, same pointer-cell protocol as issue_b, single done-bit poll,
 * no DMA-clear second wait — guest reads the reg back once into a dead
 * stack slot). */
uint32_t cd_cmd_issue_c(uint32_t arg48,uint32_t arg4c)
{
  br2_st8p(0x801bcb0c, 0);
  br2_st8p(0x801bcb18, 0x80);
  br2_st32p(0x801bcb40, 0x21020843);
  br2_st32p(0x801bcb1c, 0x1325);
  br2_st32p(0x801bcb44, br2_ld32p(0x801bcb44) | 0x8000);
  br2_st32p(0x801bcb48, arg48);
  br2_st32p(0x801bcb4c, arg4c | 0x10000);
  while ((br2_ld8p(0x801bcb0c) & 0x40) == 0) {
    if (br2_bail_pending()) return 0;
    br2_irq_poll();
  }
  br2_st32p(0x801bcb50, 0x11400100);
  (void)br2_ld32(br2_ld32p(0x801bcb50)); /* guest reads reg into dead slot */
  return 0;
}

/* cd_cmd_retry — CD retry wrapper: forwards arg0 to cd_cmd_bytes,
 * returns 1 (v0). Callers pass a buffer pointer. */
uint32_t cd_cmd_retry(uintptr_t buf)
{
  cd_cmd_bytes(buf);
  return 1;
}

/* ---- batch30: file-pump dispatch wrappers ---- */
extern void mdec_pump_b(uint32_t,uint32_t);
extern void mdec_pump_a(int32_t);
extern void cd_dispatch_tblc(void);

/* cd_fileop_dispatch — dispatch file op 1 through cd_dispatch_tbl4 table. */
void cd_fileop_dispatch(uint32_t a0)
{
  cd_dispatch_tbl4(1,a0);
}

/* cd_fileop_forward — forward to mdec_pump_b. The generated body
 * (0x8018FA30) is a pure thunk: it saves ra and jal's mdec_pump_b with the
 * caller's a0-a3 still live in gpr[4..7], so mdec_pump_b sees the original
 * arguments. mdec_pump_b only consumes a0/a1 (gpr[4]/gpr[5]); a2/a3 are
 * dead. Faithful signature is therefore (a0,a1). */
void cd_fileop_forward(uint32_t a0,uint32_t a1)
{
  mdec_pump_b(a0,a1);
}

/* cd_fileop_reset — a0==0 resets via cd_dispatch_tblc, then mdec_pump_a. */
void cd_fileop_reset(int32_t a0)
{
  if (a0 == 0) cd_dispatch_tblc();
  mdec_pump_a(a0);
}

/* cd_u16_swap_store — u16 swap-store through DAT_801bdda8 pointer. */
uint16_t cd_u16_swap_store(uint16_t val)
{
  uint32_t slot = DB32(0x801bdda8);
  uint16_t old = br2_ld16(slot);
  br2_st16(slot, val);
  return old;
}

/* cd_cmd_bytes — CD command issue: writes command/param bytes through
 * the register-pointer cells at 0x801bcb0c/10/14/18 (double
 * indirection: cell -> guest reg addr -> int8_t). Sequence: 2, a0[0],
 * a0[1], 3, a0[2], a0[3], 0x20. Disasm-verified. */
void cd_cmd_bytes(uintptr_t cmd)
{
  uint8_t *a0 = (uint8_t *)guest_at((uint32_t)cmd);
  uint8_t v;
#define CDREG(g) br2_st8p(g, v)
  v = 2;  CDREG(0x801bcb0c);
  v = a0[0]; CDREG(0x801bcb14);
  v = a0[1]; CDREG(0x801bcb18);
  v = 3;  CDREG(0x801bcb0c);
  v = a0[2]; CDREG(0x801bcb10);
  v = a0[3]; CDREG(0x801bcb14);
  v = 0x20; CDREG(0x801bcb18);
#undef CDREG
}

/* cd_dispatch_tblc — indirect dispatch: call *(u32*)(*0x801bddA0 + 0xc).
 * Same table family as cd_dispatch_tbl4: cells hold guest function
 * addresses, dispatched through call_guest. */
void cd_dispatch_tblc(void)
{
  uint32_t tbl = *(uint32_t *)guest_at(0x801bddA0);
  uint32_t fn = *(uint32_t *)guest_at(tbl + 0xc);
  (void)call_guest(fn,0,0);
}

/* cd_xchg_bde20 — get-and-set u32 @0x801bde20, returns previous. */
uint32_t cd_xchg_bde20(uint32_t val)
{
  uint32_t *p = (uint32_t *)guest_at(0x801bde20);
  uint32_t old = *p;
  *p = val;
  return old;
}

/* cd_xchg_bc850 — get-and-set u32 @0x801bc850, returns previous. */
uint32_t cd_xchg_bc850(uint32_t val)
{
  uint32_t *p = (uint32_t *)guest_at(0x801bc850);
  uint32_t old = *p;
  *p = val;
  return old;
}

/* cd_file_open_b — CD file open: when flag @0x801fc6f8 clear, cd_stamp_to_sector
 * on block @0x801feb00 then cd_file_open(fd+1,arg); returns u32
 * @0x801feb04, else -1. Disasm 0x8019917c-0x801991cc. */
extern void cd_file_open(int32_t,uint8_t *);
int32_t cd_file_open_b(uint32_t name)
{
  int32_t idx;
  if (*(uint32_t *)guest_at(0x801fc6f8) != 0) return -1;
  idx = cd_stamp_to_sector(guest_at(0x801feb00));
  cd_seek_note(3, *(uint32_t *)guest_at(0x801feb00), (uint32_t)idx);
  cd_file_open(idx + 1,(uint8_t *)guest_at(name));
  return *(int32_t *)guest_at(0x801feb04);
}

/* cd_read_param_set — CD read setup: arg -> @0x801feb38, -1 -> @0x801feb28,
 * cd_dispatch_tblc dispatch, cd_cmd_sequence(0x20000001,&block), bios_table_tramp(0).
 * Disasm 0x80199f30-0x80199f74. */
extern void cd_cmd_sequence(uint32_t,uint32_t,uint32_t,uint32_t);
void cd_read_param_set(uint32_t val)
{
  /* Faithful port of generated 0x80199F30 (76B / 4 blocks, decoded
   * insn-by-insn against generated/SLES_017.22_full_46.c:19617):
   *   frame 0x18 (sp-=0x18, ra saved at sp+0x10 — dead slot, kept via
   *   br2_guest_frame so nested generated calls see the same $sp)
   *   v0 = -1
   *   *(u32*)0x801FEB38 = a0                    // @0x80199F40 sw a0
   *   *(u32*)0x801FEB28 = 0xFFFFFFFF            // @0x80199F4C sw v0,
   *                                             //   jal cd_dispatch_tblc
   *                                             //   delay slot, ra=0x80199F50
   *   a0=0x20000001, a1=0x801FEB28 (a2/a3 live leftovers)
   *   jal cd_cmd_sequence (0x8019A00C)         // ra=0x80199F64
   *   a0=0 (delay slot), a1..a3 = cd_cmd_sequence leftovers
   *   jal bios_table_tramp (0x80199FE0)        // B0/0x5b, ra=0x80199F6C
   *   epilogue: lw ra,16(sp); sp+=0x18; jr ra — BIOS $v0 propagates.
   * All three jals go through cd_jal so a0-a3 keep callee leftovers
   * exactly like the guest: the B0/0x5b tramp forwards live a1-a3, which
   * call_guest/call_guest4 would have restored to pre-call values. */
  uint32_t fr = br2_guest_frame(0x18);
  CPUState *cpu = clean_cpu_state();
  (void)fr;
  br2_st32(0x801FEB38u, val);
  br2_st32(0x801FEB28u, 0xFFFFFFFFu);
  (void)cd_jal(0x8019A680u, 0x80199F50u);          /* jal cd_dispatch_tblc */
  if (br2_bail_pending()) { br2_guest_frame_abandon(); return; }
  cpu->gpr[4] = 0x20000001u;
  cpu->gpr[5] = 0x801FEB28u;
  (void)cd_jal(0x8019A00Cu, 0x80199F64u);          /* jal cd_cmd_sequence */
  if (br2_bail_pending()) { br2_guest_frame_abandon(); return; }
  cpu->gpr[4] = 0;
  (void)cd_jal(0x80199FE0u, 0x80199F6Cu);          /* jal bios_table_tramp */
  if (br2_bail_pending()) { br2_guest_frame_abandon(); return; }
  br2_guest_frame_free();
}

/* cd_chan_init_wrap — thin wrapper over cd_channel_main_init. Disasm verified. */
extern void cd_channel_main_init(void);
void cd_chan_init_wrap(void)
{
  cd_channel_main_init();
}

/* cd_subsystem_reset — CD subsystem reset: FUN_8018e300, then mode dispatch on
 * *(u32*)0x801bc868 (==1 -> a120c/a11e4 else 961a8; mc_session_b(0) joins
 * both paths), zero two indirect u16s @0x801bcbd0/0x801bcbdc targets,
 * FUN_8018e310(0). Disasm 0x80199070-0x801990e8. */
extern void FUN_8018e300(void);
extern void FUN_8018e310(uint32_t);
extern void cd_tbl4_dispatch3(int32_t);
extern uint32_t cd_xchg_fefb4(uint32_t);
void cd_subsystem_reset(void)
{
  uint16_t *p;
  CDFN(CDFN_SUBSYS_RESET,0);
  FUN_8018e300();
  /* generated reads mode @0x801BC868 (lui 0x801C + lw -0x3798), cells
   * @0x801BCBD0/0x801BCBDC — br2eu_func_80199070. */
  if (*(uint32_t *)guest_at(0x801bc868) == 1) {
    cd_tbl4_dispatch3(0);
    cd_xchg_fefb4(0);
  } else {
    cd_cmd_dispatch3(0);
  }
  /* mc_session_b(0) @0x80195D78 runs for BOTH mode paths (block_801990BC
   * is the join point of the mode==1 j and the else fallthrough). */
  mc_session_b(0);
  /* generated sh zero,(ptr) — u16 clears, not bytes; cells are
   * 0x801BCBD0/0x801BCBDC (lui 0x801C + lw -0x3430/-0x343C). */
  p = (uint16_t *)guest_at(*(uint32_t *)guest_at(0x801bcbd0));
  *p = 0;
  p = (uint16_t *)guest_at(*(uint32_t *)guest_at(0x801bcbdc));
  *p = 0;
  FUN_8018e310(0);
}

/* cd_probe_retry — CD probe retry: up to 5 cd_ready_probe attempts; on first
 * success installs fn-ptr triple @0x801bc848/4c/7b0 (+0 @0x801bc7b4)
 * and returns 1; else printf 0x80125638, returns 0. Disasm-verified. */
extern int32_t cd_ready_probe(void);
int32_t cd_probe_retry(void)
{
  int32_t s0;
  for (s0 = 4; s0 != -1; s0--) {
    if (cd_ready_probe() == 1) {
      /* generated sw targets are 0x801BC848/0x801BC84C/0x801BC7B0/0x801BC7B4
       * (br2eu_func_80195AA4: lui 0x801C + sw off -0x37B8/-0x37B4/-0x3850/-0x384C) */
      *(uint32_t *)guest_at(0x801bc848) = 0x80195b4c;
      *(uint32_t *)guest_at(0x801bc84c) = 0x80195b74;
      *(uint32_t *)guest_at(0x801bc7b0) = 0x80195b9c;
      *(uint32_t *)guest_at(0x801bc7b4) = 0;
      return 1;
    }
  }
  printf_forward((const char *)guest_at(0x80125638));
  return 0;
}

/* cd_channel_table_init — CD channel-table init: clears flags @0x801c47b8/cc,
 * cd_handler_triple_install, installs 7 fn ptrs @0x801c4780-0x801c4794 + base
 * @0x801c47b4, cd_bzero(0x801ff080,0x1e0), then two 0xf0-stride
 * channel blocks with ptr pairs 0x801feff0/0x801ff038 (+0x23/iter) and
 * six 0xff bytes; cd_handler_pair_install, flag=1. Disasm 0x801a36f0-0x801a3848. */
extern void cd_handler_triple_install(void);
extern uint32_t cd_bzero(uint32_t,uint32_t);
extern void cd_handler_pair_install(void);
void cd_channel_table_init(uint8_t *a0,uint8_t *a1)
{
  /* BISECT: re-delegated — clean port of the channel fn-ptr/record table
   * stores (0x801BE780+ base, 0xf0-stride channel records) broke the
   * title->menu file-load path. Keep generated until re-verified. */
  call_guest4(0x801a36f0u, guest_of(a0), guest_of(a1), 0, 0);
}

/* cd_tbl4_dispatch3 — dispatch wrapper: cd_dispatch_tbl4(3,arg). */
void cd_tbl4_dispatch3(int32_t a0)
{
  cd_dispatch_tbl4(3,(uint32_t)a0);
}

/* cd_xchg_fefb4 — get-and-set u32 @0x801fefb4, returns previous. */
uint32_t cd_xchg_fefb4(uint32_t val)
{
  uint32_t *p = (uint32_t *)guest_at(0x801fefb4);
  uint32_t old = *p;
  *p = val;
  return old;
}

/* cd_ready_probe — CD ready probe: cd_subsystem_init()==0 ->
 * (cd_reg_init_seq() < 1), else 0. Disasm 0x80195b10-0x80195b44. */
extern int32_t cd_subsystem_init(void);
extern int32_t cd_reg_init_seq(void);
int32_t cd_ready_probe(void)
{
  if (cd_subsystem_init() != 0) return 0;
  return cd_reg_init_seq() < 1 ? 1 : 0;
}

/* cd_cmd_sequence — CD command sequence: 9a3f0, crit-in, 9a378, crit-out,
 * 99fe0(0), 9a194, 9a320(a0..a3); sets flag @0x801bcc90 = 1.
 * Disasm 0x8019a00c-0x8019a094. */
extern void cd_kobj_init_a(void);
extern void cd_kobj_init_b(void);
extern void cd_subchan_table_install(void);
extern void bios_tramp_b015(uint32_t,uint32_t);
void cd_cmd_sequence(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3)
{
  /* e9fc366 clean port of 0x8019a00c regressed early boot (unknown
   * dispatch @0x80197028, frame ~171): the inline bios_svc6 B0/0x57
   * kernel-object path with synthetic $ra tokens does not reproduce the
   * generated caller-token/arg-forwarding semantics. Delegate whole
   * body to generated MIPS until the BIOS kernel boundary is modeled
   * faithfully. */
  call_guest4(0x8019a00c,a0,a1,a2,a3);
}

/* cd_channel_main_init — CD channel main init: crit-in, flag=0, 9a340/9a330
 * (2,0x801fefc0), cmd 3 handshake via *(u32*)0x801c47e0, 9a670(3,0),
 * crit-out, two jalr calls through fn slot @0x801c4784 on channel
 * bases 0x801ff080/+0xf0, clear pair @0x801fefd0, flag=1.
 * Disasm 0x801a1a40-0x801a1b08. */
extern void bios_tramp_c003(uint32_t,uint32_t);
extern void bios_tramp_c002(uint32_t,uint32_t);
void cd_channel_main_init(void)
{
  /* gpr-forwarding trampoline calls inside need live cpu->gpr that clean
   * callers cannot supply; delegate whole body to generated MIPS. */
  call_guest4(0x801a1a40u,0,0,0,0);
}

/* cd_dir_entry_probe — CD directory entry probe/free: magic-div index into
 * 0x20-stride table @*(u32*)0x801fc6c0, checks tag==4, clears link
 * fields of count entries, stores cursor @0x801fc6ac.
 * Disasm 0x80199270-0x80199314. */
uint32_t cd_dir_entry_probe(int32_t entry)
{
  int32_t a0 = entry;
  int32_t sign, hi, idx, count, i;
  uint32_t *base, *rec;
  uint32_t table_guest;
  int64_t prod;
  /* a0 and the table base are GUEST addresses (KSEG0). The index math
   * must run in guest-address space: subtracting the host pointer guest_at()
   * returns produces a wild idx and the cursor never advances. Keep the host
   * pointer only for the final record dereference. */
  table_guest = *(uint32_t *)guest_at(0x801fc6c0);
  base = (uint32_t *)guest_at(table_guest);
  a0 -= (int32_t)(*(uint32_t *)guest_at(0x801fc6c4) << 5) + (int32_t)table_guest;
  /* MIPS magic-div (0x80199290-0x801992AC): pre-shift dividend >>2, signed
   * mult by 0x82082083, then mfhi + add-back of the multiplicand, >>8, minus
   * sign of the PRE-shift dividend. The add-back term is the multiplicand
   * (a0 after >>=2), NOT the sign — dropping it yields idx=-5 instead of 5
   * and the entry is never freed, wedging the consume cursor. */
  {
    int32_t n_pre = a0;
    a0 >>= 2;
    prod = (int64_t)a0 * (int32_t)0x82082083;
    hi = (int32_t)(prod >> 32);
    sign = n_pre >> 31;
    idx = ((hi + a0) >> 8) - sign;
  }
  rec = base + (idx << 5) / 4;   /* idx*32 bytes = idx*8 u32 */
  rec = (uint32_t *)((uint8_t *)base + (idx << 5));
  if (*(int16_t *)rec != 4) { return 1; }
  count = (int16_t)*(uint16_t *)((uint8_t *)rec + 6);
  a0 = 0;   /* MIPS 0x801992D8: move a0,zero — cursor addend baseline */
  if (count > 0) {
    for (i = 0; i < count; i++) {
      uint32_t *r = (uint32_t *)((uint8_t *)base + ((i + idx) << 5));
      *(int16_t *)r = 0;
    }
    a0 = count;   /* free-loop leaves a0 == count (0x801992E4 addiu) */
  }
  *(uint32_t *)guest_at(0x801fc6ac) = (uint32_t)(a0 + idx);
  return 0;
}

/* cd_xchg_fefb8 — get-and-set u32 @0x801fefb8, returns previous. */
uint32_t cd_xchg_fefb8(uint32_t val)
{
  uint32_t *p = (uint32_t *)guest_at(0x801fefb8);
  uint32_t old = *p;
  *p = val;
  return old;
}

/* BIOS vector trampolines: jr $t2 -> kernel entries 0xa0/0xb0/0xc0
 * with t1 = service number and a0/a1 forwarded. Routed through
 * bios_svc (table,service,a0,a1) — same boundary family as
 * bios_table_tramp/9a670. */
uint32_t bios_svc(uint32_t,uint32_t,uint32_t,uint32_t);
void bios_tramp_b015(uint32_t a0,uint32_t a1) { bios_svc(0xb0,0x15,a0,a1); }
void bios_tramp_c002(uint32_t a0,uint32_t a1) { bios_svc(0xc0,2,a0,a1); }
void bios_tramp_c003(uint32_t a0,uint32_t a1) { bios_svc(0xc0,3,a0,a1); }
/* 0x8019CFB0: `li t2,0xB0; li t1,0x0A; jr t2` — B0 svc 0x0A kernel-call
 * trampoline forwarding live a0-a3 (svc_call re-publishes t1/t2 itself).
 * Sole caller: cd_mode_apply_deferred's chunk loop (jal, ra=0x8019CF48). */
uint32_t bios_tramp_b00a(uint32_t a0,uint32_t a1) { return bios_svc(0xb0,0x0a,a0,a1); }

/* ---- BIOS kernel-vector trampolines (16B `li t2,<vec>; jr t2; li t1,<svc>`) ----
 * Guest shape (e.g. 0x8019B380): t2 = vector (0xB0/0xA0), t1 = service, jr t2
 * with the caller's a0-a3 forwarded untouched; BIOS $v0 comes back in $v0.
 * bios_svc6 re-publishes t0/t1/t2 and runs the real BIOS through svc_call, so
 * the four-arg forward below is exact for both the clean_table wrapper
 * (gpr[4..7] marshalled) and direct C callers. */
uint32_t bios_tramp_b008(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3) { return bios_svc6(0xb0,0x08,a0,a1,a2,a3); }
uint32_t bios_tramp_b009(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3) { return bios_svc6(0xb0,0x09,a0,a1,a2,a3); }
uint32_t bios_tramp_b00c(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3) { return bios_svc6(0xb0,0x0c,a0,a1,a2,a3); }
uint32_t bios_tramp_b012(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3) { return bios_svc6(0xb0,0x12,a0,a1,a2,a3); }
uint32_t bios_tramp_b013(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3) { return bios_svc6(0xb0,0x13,a0,a1,a2,a3); }
uint32_t bios_tramp_b014(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3) { return bios_svc6(0xb0,0x14,a0,a1,a2,a3); }
uint32_t bios_tramp_b018(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3) { return bios_svc6(0xb0,0x18,a0,a1,a2,a3); }
uint32_t bios_tramp_b019(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3) { return bios_svc6(0xb0,0x19,a0,a1,a2,a3); }
uint32_t bios_tramp_b032(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3) { return bios_svc6(0xb0,0x32,a0,a1,a2,a3); }
uint32_t bios_tramp_b033(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3) { return bios_svc6(0xb0,0x33,a0,a1,a2,a3); }
uint32_t bios_tramp_b034(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3) { return bios_svc6(0xb0,0x34,a0,a1,a2,a3); }
uint32_t bios_tramp_b036(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3) { return bios_svc6(0xb0,0x36,a0,a1,a2,a3); }
uint32_t bios_tramp_b042(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3) { return bios_svc6(0xb0,0x42,a0,a1,a2,a3); }
uint32_t bios_tramp_b043(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3) { return bios_svc6(0xb0,0x43,a0,a1,a2,a3); }
uint32_t bios_tramp_b045(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3) { return bios_svc6(0xb0,0x45,a0,a1,a2,a3); }
uint32_t bios_tramp_b04c(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3) { return bios_svc6(0xb0,0x4c,a0,a1,a2,a3); }
uint32_t bios_tramp_a033(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3) { return bios_svc6(0xa0,0x33,a0,a1,a2,a3); }
uint32_t bios_tramp_a072(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3) { return bios_svc6(0xa0,0x72,a0,a1,a2,a3); }
uint32_t bios_tramp_a0ab(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3) { return bios_svc6(0xa0,0xab,a0,a1,a2,a3); }
uint32_t bios_tramp_a0ac(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3) { return bios_svc6(0xa0,0xac,a0,a1,a2,a3); }

/* cd_mode_apply / cd_mode_apply_wrap — 0x8019E400 / 0x8019E410, B0 svc
 * 0x4A / 0x4B trampolines (renames.map canonical names; bind.c carries weak
 * C-call fallbacks for the standalone selftest). */
uint32_t cd_mode_apply(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3) { return bios_svc6(0xb0,0x4a,a0,a1,a2,a3); }
uint32_t cd_mode_apply_wrap(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3) { return bios_svc6(0xb0,0x4b,a0,a1,a2,a3); }

/* cpu_critical_enter / cpu_critical_exit — 0x8018E300 / 0x8018E310:
 * `li a0,1|2; syscall 0` (EnterCriticalSection/DI, ExitCriticalSection/EI).
 * Routed through the real psx_syscall via the clean_bridge FUN_ wrappers so
 * COP0 SR IEc/IM[2] change exactly as generated. DI returns old IEc in $v0;
 * EI preserves $v0 — both return the live gpr[2] for faithfulness. */
uint32_t cpu_critical_enter(void)
{
  CPUState *c = clean_cpu_state();
  FUN_8018e300();
  return c ? c->gpr[2] : 0;
}
/* cpu_critical_exit — critical-section exit: FUN_8018e310(0) (pairs with cpu_critical_enter's FUN_8018e300); a0 unused, returns v0. */
uint32_t cpu_critical_exit(uint32_t a0)
{
  CPUState *c = clean_cpu_state();
  (void)a0;
  FUN_8018e310(0);
  return c ? c->gpr[2] : 0;
}

/* cd_extent_len — 0x801A27D0: leaf span calc over a ctx/extent record.
 * a0 = record guest addr:
 *   v0 = lbu(rec+0xE3); a1 = lbu(rec+0xE9); a0 = lhu(rec+0xEC)
 *   ret = (((v0+1)>>1)<<2) + ((((a1*5+3)&0xFFC)+4)) + a0
 * Decode verified insn-for-insn against generated full_49. */
uint32_t cd_extent_len(uint32_t a0)
{
  uint32_t b3  = br2_ld8(a0 + 0xe3u);
  uint32_t b9  = br2_ld8(a0 + 0xe9u);
  uint32_t hec = br2_ld16(a0 + 0xecu);
  uint32_t t   = ((b3 + 1u) >> 1) << 2;
  uint32_t u   = (((b9 * 5u) + 3u) & 0xffcu) + 4u;
  return t + u + hec;
}

/* cd_subchan_table_install — CD sub-channel table install: crit-in, stores code
 * addrs 0x801aa244/0x801aa2ac into dispatch cells @0x801feb44/48,
 * clears @0x801feb40/@0x801feb4c, then
 * bios_tramp_c003(1,0x801febc0) + bios_tramp_c002(1,0x801febc0), crit-out.
 * Disasm 0x8019a194-0x8019a204. */
void cd_subchan_table_install(void)
{
  uint32_t s0 = 0x801feb40;
  FUN_8018e300();
  *(uint32_t *)guest_at(0x801feb44) = 0x801aa244;
  *(uint32_t *)guest_at(0x801feb48) = 0x801aa2ac;
  *(uint32_t *)guest_at(0x801feb40) = 0;
  *(uint32_t *)guest_at(0x801feb4c) = 0;
  bios_tramp_c003(1,s0);
  bios_tramp_c002(1,s0);
  FUN_8018e310(0);
}

/* cd_dispatch_tbl8 — indirect dispatch: call *(u32*)(*0x801bddA0 + 8). */
int cd_dispatch_tbl8(int a0,uint32_t a1)
{
  uint32_t tbl = CDB32G(0x801bdda0);
  uint32_t fn = *(uint32_t *)guest_at(tbl + 8);
  return (int)call_guest(fn,(uint32_t)a0,a1);
}

/* Two-level CD register cells: cell @0x801bcbXX holds a guest
 * pointer; the register int8_t/half/word lives at *cell. */
static uint8_t *cdcell_u8(uint32_t cell)
{ return (uint8_t *)guest_at(*(uint32_t *)guest_at(cell)); }
static uint16_t *cdcell_u16(uint32_t cell)
{ return (uint16_t *)guest_at(*(uint32_t *)guest_at(cell)); }
static uint32_t cdcell_u32(uint32_t cell)
{ return *(uint32_t *)guest_at(*(uint32_t *)guest_at(cell)); }

/* cd_reg_init_seq — CD register init sequence: threshold fields on
 * channel block, then command bytes 2/3/2 with params 0x80/0/0x80/0
 * and status 0x20 through cells 0x801bcb0c/10/14/18.
 * Disasm 0x8019737c-0x80197464. */
int32_t cd_reg_init_seq(void)
{
  /* 0x801bcb20 is a pointer cell into the 0x1F8010xx CD drive regs —
   * indexed u16 accesses are MMIO and must use br2_ld/st, not guest_at. */
  uint32_t base;
  uint16_t v0;
  base = DB32G(0x801bcb20);
  v0 = br2_ld16(base + 0xdc * 2);        /* +0x1b8 */
  if (v0 == 0) {
    v0 = br2_ld16(base + 0xdd * 2);      /* +0x1ba */
    if (v0 == 0) {
      /* every generated path reaching the +0x1b0/+0x1b2 stores reloads
       * li v0,0x3fff (@0x801973A4 delay / @0x801973B8) — the stored value
       * is ALWAYS 0x3fff, never the raw lh result (br2eu_func_8019737C). */
      br2_st16(base + 0xc0 * 2, 0x3fff);  /* +0x180 */
      br2_st16(base + 0xc1 * 2, 0x3fff);  /* +0x182 */
    }
  }
  br2_st16(base + 0xd8 * 2, 0x3fff);      /* +0x1b0 */
  br2_st16(base + 0xd9 * 2, 0x3fff);      /* +0x1b2 */
  br2_st16(base + 0xd5 * 2, 0xc001);      /* +0x1aa */
  /* command issue @0x801973CC-0x8019745C: the guest builds {0x80,0,0x80,0}
   * on its stack and writes UNCONDITIONALLY through the pointer cells —
   * cmd 2 -> *cb0c, params -> *cb14, *cb18, *cb10, *cb14, cmd 3 -> *cb0c,
   * 0x20 -> *cb18. There are no readiness reads or conditionals; the old
   * br2_ld8-gated form both issued extra MMIO reads and skipped writes. */
  br2_st8p(0x801bcb0c, 2);
  br2_st8p(0x801bcb14, 0x80);
  br2_st8p(0x801bcb18, 0);
  br2_st8p(0x801bcb0c, 3);
  br2_st8p(0x801bcb10, 0x80);
  br2_st8p(0x801bcb14, 0);
  br2_st8p(0x801bcb18, 0x20);
  return 0;
}

/* cd_subsystem_init — CD subsystem init: banner print, global clears,
 * cd_dispatch_tblc dispatch, cd_dispatch_tbl8(2,0x801979f8), N-command
 * handshake polling status&7, channel setup via cd_cmd_issue,
 * returns -1 on command failure else cd_result_wait_a()==2 ? 0 : -1.
 * Disasm 0x801974b8-0x80197690. */
int32_t cd_subsystem_init(void)
{
  uint8_t a0 = 1, a1 = 0, v1 = 7;
  uint8_t *c;
  CDFN(CDFN_SUBSYS_INIT,0);
  cd_str_print(0x80125898);
  printf_forward((const char *)guest_at(0x801258a4),
               (void *)guest_at(0x801bcb28));
  *(uint8_t *)guest_at(0x801bc865) = 0;
  *(uint8_t *)guest_at(0x801bc864) = 0;
  *(uint32_t *)guest_at(0x801bc84c) = 0;
  *(uint32_t *)guest_at(0x801bc848) = 0;
  *(uint32_t *)guest_at(0x801bc858) = 0;
  *(uint32_t *)guest_at(0x801bc854) = 0;
  cd_dispatch_tblc();
  cd_dispatch_tbl8(2,0x801979f8);
  br2_st8p(0x801bcb0c, 1);
  while ((br2_ld8p(0x801bcb18) & 7) != 0) {
    if (br2_bail_pending()) return -1;
    br2_irq_poll();
    br2_st8p(0x801bcb0c, a0);
    br2_st8p(0x801bcb18, v1);
    br2_st8p(0x801bcb14, v1);
  }
  {
    uint8_t *q = (uint8_t *)guest_at(0x801bcb24);
    if (q) { q[2] = 0; q[1] = q[2]; q[0] = 2; }
    br2_st8p(0x801bcb0c, 0);
    br2_st8p(0x801bcb18, 0);
    br2_st32p(0x801bcb1c, 0x1325);
    /* guest passes a3=0 (async flag clear) and IGNORES this call's return —
     * there is no v0 test between the jal @0x80197600 and the 0x801bc854
     * reload @0x80197604 (br2eu_func_801974B8). */
    cd_cmd_issue(1,0,0,0);
  }
  if (*(uint32_t *)guest_at(0x801bc854) & 0x10) {
    cd_cmd_issue(1,0,0,0);   /* return also ignored @0x8019761C-0x8019762C */
  }
  if (cd_cmd_issue(0xa,0,0,0) != 0) return -1;
  if (cd_cmd_issue(0xc,0,0,0) != 0) return -1;
  return cd_result_wait_a(0,0) == 2 ? 0 : -1;
}

/* cd_kobj_init_a — kernel-object init A. Faithful port of generated
 * 0x8019a3f0: saves $ra to cell 0x801FEB60, DI, inline BIOS tramp
 * (t1=0x57, t2=0xB0, jalr t2 -> B0/0x57 kernel-object alloc, forwards
 * live a0-a3), loads sub=*(obj+0x16c), clears 9 words at sub+0x62c,
 * bios_tramp_a044, EI, restores $ra from the cell. The BIOS svc runs
 * through bios_svc6 -> psx_dispatch_call on the real recompiled BIOS;
 * $ra is set to the guest jalr link 0x8019A410 because OpenBIOS kernel
 * calls validate $ra as a caller-code token. */
extern void bios_tramp_a044(void);
void cd_kobj_init_a(void)
{
  /* e9fc366 clean port of 0x8019a3f0 regressed early boot: the inline
   * bios_svc6 B0/0x57 kernel-object alloc with a synthetic $ra token
   * corrupts kernel-object state (boot dies in the 0x801970xx CD-event
   * wait -> unknown dispatch). Delegate whole body to generated MIPS —
   * the BIOS kernel boundary needs live gpr/ra semantics clean C lacks. */
  call_guest4(0x8019a3f0,0,0,0,0);
}

/* cd_kobj_init_b — kernel-object init B. Faithful port of generated
 * 0x8019a378: saves $ra to cell 0x801FEB50, DI, inline BIOS tramp
 * (t1=0x57, t2=0xB0 -> B0/0x57, live a0-a3), sub=*(obj+0x16c),
 * stores sub+0x884 -> @0x801FEB58 and sub+0x894 -> @0x801FEB5C,
 * clears 11 words at sub+0x594, bios_tramp_a044, restores $ra.
 * No crit-out (matches guest). $ra set to the jalr link 0x8019A398
 * for the OpenBIOS caller-token check, same as cd_kobj_init_a. */
void cd_kobj_init_b(void)
{
  /* Same e9fc366 regression as cd_kobj_init_a — delegate to generated. */
  call_guest4(0x8019a378,0,0,0,0);
}

/* bios_tramp_a044 — BIOS svc A0/0x44 trampoline. */
void bios_tramp_a044(void) { br2_bios_tramp(0xa0,0x44); }

/* cd_dir_table_set — set dir table base/cursor: a0 -> @0x801fc6c0,
 * a1 -> @0x801fc6c4, then cd_dir_reset. Disasm 0x80195800-24. */
extern void cd_dir_reset(void);
void cd_dir_table_set(uint32_t a0,uint32_t a1)
{
  CDFN(CDFN_DIR_TABLE_SET,a0);
  *(uint32_t *)guest_at(0x801fc6c0) = a0;
  *(uint32_t *)guest_at(0x801fc6c4) = a1;
  cd_dir_reset();
}

/* cd_dir_read_wrap — wrapper: cd_dir_read(1,a0). */
extern uint32_t cd_dir_read(uint32_t,uint32_t);
void cd_dir_read_wrap(uint32_t a0) { cd_dir_read(1,a0); }

/* cd_u16_table_store — u16 table store: a2==0 -> *(u16*)(*0x801c-0x2150 +
 * a0*2) = a1; else -> same + (a1 >> *0x801c-0x2128). Disasm verified. */
void cd_u16_table_store(uint32_t a0,uint32_t a1,int32_t a2)
{
  uint16_t *t = (uint16_t *)guest_at(*(uint32_t *)guest_at(0x801cdeac - 0x0));
  uint16_t *base = (uint16_t *)guest_at(*(uint32_t *)guest_at(0x801c0000 - 0x2150));
  (void)t;
  if (a2 == 0) {
    base[a0] = (uint16_t)a1;
  } else {
    /* srlv: shift amount is the cell value & 31 (br2eu_func_8019BCAC
     * @0x8019BCE0 PGXP_ALU 0x00651806). */
    uint32_t sh = *(uint32_t *)guest_at(0x801c0000 - 0x2128) & 31;
    base[a0] = (uint16_t)(a1 >> sh);
  }
}

/* cd_reg_window_update — cd_chan_attr_push(); cd_reg_window_u16(1,*(u32*)a0). */
extern void cd_chan_attr_push(uint32_t);
extern void cd_reg_window_u16(int32_t,uint32_t);
void cd_reg_window_update(uint8_t *a0)
{
  /* a0 is a host pointer into guest memory; cd_chan_attr_push takes a
   * raw guest address — translate, don't truncate the host pointer. */
  cd_chan_attr_push(br2_gaddr(a0));
  cd_reg_window_u16(1,*(uint32_t *)a0);
}

/* cd_subsys_reset_wrap — cd_mode_apply_seq(); cd_mode_reset_seq(); bios_tramp_a070(). */
extern void cd_mode_apply_seq(uint32_t);
extern void cd_mode_reset_seq(void);
extern void bios_tramp_a070(uint32_t,uint32_t);
void cd_subsys_reset_wrap(void)
{
  cd_mode_apply_seq(0); cd_mode_reset_seq(); bios_tramp_a070(0,0);
}

/* cd_bzero — memset: zero a1 bytes at a0; returns a0, or 0 when
 * a0==0 or a1<=0. Disasm 0x801a09b0-0x801a09d8. */
uint32_t cd_bzero(uint32_t a0,uint32_t a1)
{
  uint8_t *p;
  uint32_t n = a1;
  if (a0 == 0 || (int32_t)a1 <= 0) return 0;
  p = (uint8_t *)guest_at(a0);
  if (p == NULL) return 0;
  while (n--) *p++ = 0;
  return a0;
}

/* cd_handler_pair_install — install handler pair: @0x801fefc0=0,
 * @0x801fefc4=0x801a189c, @0x801fefc8=0x801a1834, @0x801fefcc=0.
 * Disasm verified. */
void cd_handler_pair_install(void)
{
  *(uint32_t *)guest_at(0x801fefc0) = 0;
  *(uint32_t *)guest_at(0x801fefc4) = 0x801a189c;
  *(uint32_t *)guest_at(0x801fefc8) = 0x801a1834;
  *(uint32_t *)guest_at(0x801fefcc) = 0;
}

/* cd_handler_triple_install — install three handler addrs:
 * 0x801a3d84->0x801be798, 0x801a41d8->0x801be79c, 0x801a3e94->0x801be7a0.
 * Disasm 0x801a3d50-0x801a3d84. */
void cd_handler_triple_install(void)
{
  *(uint32_t *)guest_at(0x801be798) = 0x801a3d84;
  *(uint32_t *)guest_at(0x801be79c) = 0x801a41d8;
  *(uint32_t *)guest_at(0x801be7a0) = 0x801a3e94;
}

/* cd_ext_init_once — one-shot CD ext-table init: guard @0x801bdea0,
 * crit-in, cd_ext_table_init(0x8019b7e0), FUN_8019b380(0xf0000009,0x20,
 * 0x2000,0) -> FUN_8019b390 -> @0x801bde38, crit-out.
 * Disasm 0x8019b308-0x8019b370. */
extern int32_t cd_ext_table_init(uint32_t);
extern uint32_t br2_call_generated(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
void cd_ext_init_once(void)
{
  /* OpenEvent/EnableEvent via FUN_8019b380/390 -> bios_svc6 needs the
   * guest $ra caller-token semantics the generated CPS frame supplies;
   * the clean port livelocked versus boot in the CD-event wait. Delegate
   * whole body to generated MIPS. (The 0x8019B7E0 callback arg was
   * verified against generated: a0 = 0x801A0000-0x4820.) */
  call_guest4(0x8019b308u,0,0,0,0);
}

/* cd_chan_block_boot — channel block @0x801febe0 init: clears +0/4/8/0xc/
 * 0x44/0x54, cd_job_slots_reset, +0x14=-1, +0x48/+0x4c=1, +0x50=old+0x54,
 * cd_hw_chan_init, cd_chan_block_init(7,0x801a0110). Disasm 0x8019e680-e6e8. */
extern void cd_job_slots_reset(void);
extern void cd_hw_chan_init(void);
extern void cd_chan_block_init(int32_t,uint32_t);
void cd_chan_block_boot(void)
{
  uint8_t *s0 = (uint8_t *)guest_at(0x801febe0);
  uint32_t v1;
  if (s0 == NULL) return;
  *(uint32_t *)(s0 + 0xc) = 0;
  cd_job_slots_reset();
  *(uint32_t *)(s0 + 0x44) = 0;
  *(uint32_t *)s0 = 0;
  *(uint32_t *)(s0 + 4) = 0;
  *(uint32_t *)(s0 + 8) = 0;
  *(uint32_t *)(s0 + 0x54) = 0;
  v1 = *(uint32_t *)(s0 + 0x54);
  *(uint32_t *)(s0 + 0x14) = 0xffffffff;
  *(uint32_t *)(s0 + 0x4c) = 1;
  *(uint32_t *)(s0 + 0x48) = 1;
  *(uint32_t *)(s0 + 0x50) = v1;
  cd_hw_chan_init();
  cd_chan_block_init(7,0x801a0110);
}

/* cd_mode_set_0e — CD mode/flag set: cd_mode_flag_set(0xe,&int8_t,0);
 * when bit 0x100 set, clears @0x801fc6f8 and runs 961a8/95d78 pair;
 * always cd_mode_flag_set(0x1b,0,0). Disasm 0x80198f60-0x80198fdc.
 * Faithful port: the Setmode param int8_t lives on a real guest frame
 * (br2_guest_frame 0x20, int8_t at sp+16 = low8(a0)) so generated
 * cd_cmd_issue sees the same guest address the guest's sp+0x10 produces —
 * a host stack &b was the original wedge (mode 0x00 delivered instead of
 * the computed int8_t). Returns v0 = final cd_mode_flag_set(0x1b,0,0). */
int cd_mode_set_0e(uint32_t s0)
{
  uint32_t fr;
  int ret;
  CDFN(CDFN_MODE_SET_0E,s0);
  fr = br2_guest_frame(0x20);
  br2_st8(fr + 16, (uint8_t)s0);
  (void)cd_mode_flag_set(14, (int)(fr + 16), 0);
  if (br2_bail_pending()) { br2_guest_frame_free(); return 0; }
  if ((s0 & 0x100u) == 0) {
    ret = (int)cd_mode_flag_set(27, 0, 0);
  } else {
    /* guest: v0 = (s0 & 0x20) ? 0 : 1 -> sw @0x8020c6f8 (delay-slot li 1) */
    br2_st32(0x8020c6f8, (s0 & 0x20u) ? 0u : 1u);
    (void)cd_cmd_dispatch3(0x801990f0u);
    if (br2_bail_pending()) { br2_guest_frame_free(); return 0; }
    (void)mc_session_b(0x80198fe4u);
    if (br2_bail_pending()) { br2_guest_frame_free(); return 0; }
    ret = (int)cd_mode_flag_set(27, 0, 0);
  }
  br2_guest_frame_free();
  return ret;
}

/* cd_read_params_store — CD read param block: cd_read_param_block(1), stores
 * flag&1, a3, stack arg and clears into @0x801fc6xx cells.
 * Disasm 0x801991e0-0x8019925c. */
extern void cd_read_param_block(uint32_t,uint32_t,uint32_t);
void cd_read_params_store(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3,uint32_t s2)
{
  cd_read_param_block(1,a1,a2);
  *(uint32_t *)guest_at(0x801fc6b0) = 0;
  *(uint32_t *)guest_at(0x801feb10) = a3;
  *(uint32_t *)guest_at(0x801fc688) = a0 & 1;
  *(uint32_t *)guest_at(0x801fc698) = 0;
  *(uint32_t *)guest_at(0x801fc690) = 0;
  *(uint16_t *)guest_at(0x801fc684) = 0;
  *(uint32_t *)guest_at(0x801fc680) = 0;
  *(uint32_t *)guest_at(0x801feb14) = s2;
  (void)a1; (void)a2;
}

/* cd_read_param_block — store a0 -> @0x801fc6b8, a1 -> @0x801fc694,
 * a2 -> @0x801fc6b4 (channel stuck-entry reset flag read by
 * cd_channel_poll). Disasm 0x80199420-0x8019943c: three word stores,
 * a2 lands on -0x394c($at)=0x801fc6b4. */
void cd_read_param_block(uint32_t a0,uint32_t a1,uint32_t a2)
{
  *(uint32_t *)guest_at(0x801fc6b8) = a0;
  *(uint32_t *)guest_at(0x801fc694) = a1;
  *(uint32_t *)guest_at(0x801fc6b4) = a2;
}

/* cd_ext_table_init — faithful port of generated 0x8019bee0 (CPS chain
 * verified): tail-dispatch cd_dispatch_tbl4(4, a0) — a0=4, a1=caller a0.
 * Guest v0 = the dispatch result, propagated through the int return. */
int32_t cd_ext_table_init(uint32_t a0)
{
  return cd_dispatch_tbl4(4,(uint32_t)a0);
}

/* cd_chan_block_init — indirect dispatch tbl+0x14: call_guest(fn,a0,a1). */
void cd_chan_block_init(int32_t a0,uint32_t a1)
{
  uint32_t tbl = CDB32G(0x801bdda0);
  uint32_t fn = *(uint32_t *)guest_at(tbl + 0x14);
  (void)call_guest(fn,(uint32_t)a0,a1);
}

/* cd_mode_reset_seq — 99fe0(0), 9e410, 8e310 sequence wrapper.
 * Disasm 0x8019e39c-0x8019e3cc. */
extern uint32_t cd_mode_apply_wrap(uint32_t,uint32_t,uint32_t,uint32_t);
void cd_mode_reset_seq(void)
{
  /* c784bf1 clean port of 0x8019e39c regressed versus boot (same
   * bios_table_tramp/BIOS-svc boundary family as the e9fc366 break):
   * delegate whole body to generated MIPS. */
  call_guest4(0x8019e39cu,0,0,0,0);
}

/* cd_dir_reset — CD dir reset: a1=*0x801fc6c4, clears
 * @0x801fc6ac/a8/a4/69c, cd_dir_clear(0,a1), @0x801fc68c=0,
 * @0x801fc684(u16)=0, @0x801fc680=0. Disasm verified. */
extern void cd_dir_clear(uint32_t,uint32_t);
void cd_dir_reset(void)
{
  uint32_t a1 = *(uint32_t *)guest_at(0x801fc6c4);
  CDFN(CDFN_DIR_RESET,0);
  *(uint32_t *)guest_at(0x801fc6ac) = 0;
  *(uint32_t *)guest_at(0x801fc6a8) = 0;
  *(uint32_t *)guest_at(0x801fc6a4) = 0;
  *(uint32_t *)guest_at(0x801fc69c) = 0;
  cd_dir_clear(0,a1);
  *(uint32_t *)guest_at(0x801fc68c) = 0;
  *(uint16_t *)guest_at(0x801fc684) = 0;
  *(uint32_t *)guest_at(0x801fc680) = 0;
}

/* cd_mode_apply_seq — CD mode apply: 99fe0(0), 8e300, v0=99ffc();
 * if !v0 arg=0; 9e400(arg), 9e5cc, 9e4c8, 9e55c, 8e310(0).
 * Disasm 0x8019e330-0x8019e394. */
extern uint32_t cd_mode_apply(uint32_t,uint32_t,uint32_t,uint32_t);
extern int32_t cd_status_bcc90(void);
void cd_mode_apply_seq(uint32_t a0)
{
  /* c784bf1 clean port of 0x8019e330 regressed versus boot — delegate
   * whole body to generated MIPS (BIOS trampoline boundary). */
  (void)call_guest4(0x8019e330u,a0,0,0,0);
}

/* bios_tramp_a070 — BIOS svc A0/0x70 trampoline. */
void bios_tramp_a070(uint32_t a0,uint32_t a1) { bios_svc(0xa0,0x70,a0,a1); }

/* cd_job_slots_reset — store -1 @0x801be768. Disasm 0x801a0a00-0x801a0a0c. */
void cd_job_slots_reset(void)
{
  *(uint32_t *)guest_at(0x801be768) = 0xffffffff;
}

/* cd_chan_flags_to_dir — channel flags -> dir entry u16 stores:
 * a0 guest addr. (a1=*a0)&2 or a1<1 -> base=*0x801bdeb0:
 * u16(a0+8)->base+0x184 and @0x801bde58; (a1&4)&&a1>=1 ->
 * u16(a0+0xa)->base+0x186 and @0x801bde5a. Disasm verified. */
void cd_chan_flags_to_dir(uint32_t a0)
{
  uint8_t *b = (uint8_t *)guest_at(a0);
  uint32_t a1;
  uint32_t base;
  int lo;
  if (b == NULL) return;
  a1 = *(uint32_t *)b;
  /* guest uses sltiu (unsigned a1<1 == a1==0) for the "all" flag, and the
   * second block is gated by lo||(a1&4) — @0x8019CADC branches on lo!=0
   * straight into the +0x186 block, skipping the &4 test (br2eu_func_
   * 8019CAA0). The clean port had it inverted (!lo&&(a1&4)). */
  lo = (a1 < 1u);
  if (lo || (a1 & 2)) {
    base = *(uint32_t *)guest_at(0x801bdeb0);
    CDMMIO16_ST(base,0xc2,*(uint16_t *)(b + 8));      /* +0x184 */
    *(uint16_t *)guest_at(0x801bde58) = *(uint16_t *)(b + 8);
  }
  if (lo || (a1 & 4)) {
    base = *(uint32_t *)guest_at(0x801bdeb0);
    CDMMIO16_ST(base,0xc3,*(uint16_t *)(b + 0xa));    /* +0x186 */
    *(uint16_t *)guest_at(0x801bde5a) = *(uint16_t *)(b + 0xa);
  }
}

/* cd_file_open — sector->BCD time encode: magic-multiply division chain
 * writes 3 bytes to buf. Disasm 0x801961ec-0x801962ec (mult/mflo/mfhi
 * pipeline preserved literally). */
void cd_file_open(int32_t a0, uint8_t *v0)
{
  int32_t a2, a3, t0, q;
  int64_t p;
  a0 += 0x96;
  p = (int64_t)a0 * (int32_t)0x1b4e81b5;
  a3 = ((int32_t)(p >> 32) >> 3) - (a0 >> 31);
  p = (int64_t)a3 * (int32_t)0x88888889;
  a2 = (int32_t)(p >> 32);
  a0 -= a3 * 75;
  p = (int64_t)a0 * (int32_t)0x66666667;
  t0 = ((a2 + a3) >> 5) - (a3 >> 31);
  a3 -= t0 * 60;
  q = ((int32_t)(p >> 32) >> 2) - (a0 >> 31);
  a0 -= q * 10;
  a2 = q * 16;
  p = (int64_t)a3 * (int32_t)0x66666667;
  a2 += a0;
  v0[2] = (uint8_t)a2;
  q = ((int32_t)(p >> 32) >> 2) - (a3 >> 31);
  a3 -= q * 10;
  v0[1] = (uint8_t)(q * 16 + a3);
  p = (int64_t)t0 * (int32_t)0x66666667;
  q = ((int32_t)(p >> 32) >> 2) - (t0 >> 31);
  t0 -= q * 10;
  v0[0] = (uint8_t)(q * 16 + t0);
}

/* cd_channel_poll — CD channel-table poll: entry = base@0x801fc6c0 +
 * idx@0x801fc6ac *0x20; state 1 -> maybe reset (if @0x801fc6b4),
 * state 2 -> state=4, *p1 = base + idx2@0x801fc6c4*0x20 + idx*0x7e0,
 * *p2 = entry (guest addrs). Returns 0 on handled, 1 while polling,
 * 2 on other states. Disasm 0x80199360-0x80199414. */
int32_t cd_channel_poll(uint32_t *out_rec, uint32_t *out_ent)
{
  uint32_t base = *(uint32_t *)guest_at(0x801fc6c0);
  uint32_t idx = *(uint32_t *)guest_at(0x801fc6ac);
  uint8_t *e = (uint8_t *)guest_at(base + idx * 0x20);
  if (*(uint16_t *)e == 1) {
    /* Generated 0x8019938C-0x801993A4: the idx reset (0x801FC6AC=0) sits in
     * the delay slot of the 0x801FC6B4 test, so it runs UNCONDITIONALLY on
     * state==1; only the entry-state clear is gated on 0x801FC6B4!=0. */
    uint32_t clr = *(uint32_t *)guest_at(0x801fc6b4);
    *(uint32_t *)guest_at(0x801fc6ac) = 0;
    if (clr != 0) *(uint16_t *)e = 0;
    idx = *(uint32_t *)guest_at(0x801fc6ac);
    e = (uint8_t *)guest_at(base + idx * 0x20);
  }
  if (*(uint16_t *)e != 2) { return 1; }
  *(uint16_t *)e = 4;
  *out_rec = base + *(uint32_t *)guest_at(0x801fc6c4) * 0x20 + idx * 0x7e0;
  *out_ent = base + idx * 0x20;
  return 0;
}

/* cd_chan_mode_set — channel mode set: mode 0 clears state @0x801bde44 and
 * bit7 of u16 @ptr@0x801bdeb0+0x1aa; mode 1 sets state=1 + bit7 when
 * @0x801bde48==1 or cd_range_table_member(@0x801bde4c)==0, else clears like mode 0;
 * other modes return current state. Disasm 0x8019bf10-0x8019bfd4. */
extern int32_t cd_range_table_member(uint32_t);
int32_t cd_chan_mode_set(uint32_t mode)
{
  uint32_t blk;
  uint16_t v0;
  if (mode == 0 || mode == 1) {
    int do_set = 0;
    if (mode == 0) {
      do_set = 0;
    } else {
      if (*(uint32_t *)guest_at(0x801bde48) == 1) do_set = 1;
      else if (cd_range_table_member(*(uint32_t *)guest_at(0x801bde4c)) == 0) do_set = 1;
    }
    blk = *(uint32_t *)guest_at(0x801bdeb0);
    v0 = CDMMIO16(blk,0xd5);                    /* +0x1aa */
    if (do_set) {
      *(uint32_t *)guest_at(0x801bde44) = 1;
      CDMMIO16_ST(blk,0xd5,(uint16_t)(v0 | 0x80));
    } else {
      *(uint32_t *)guest_at(0x801bde44) = 0;
      CDMMIO16_ST(blk,0xd5,(uint16_t)(v0 & 0xff7f));
    }
  }
  return (int32_t)*(uint32_t *)guest_at(0x801bde44);
}

/* cd_dir_clear — clear a1 consecutive 0x20-stride entries of the dir
 * table @*(u32*)0x801fc6c0 starting at index a0.
 * Disasm 0x80199320-0x80199350. */
void cd_dir_clear(uint32_t a0, uint32_t a1)
{
  uint32_t base = *(uint32_t *)guest_at(0x801fc6c0);
  uint32_t i;
  for (i = 0; i < a1; i++)
    *(uint32_t *)guest_at(base + (a0 + i) * 0x20) = 0;
}

/* cd_reg_win_rmw_cc_cd — trampoline: cd_reg_window_rmw(a0,a1,0xcc,0xcd).
 * Disasm 0x8019cb20-0x8019cb40. */
extern uint32_t cd_reg_window_rmw(uint32_t,uint32_t,uint32_t,uint32_t);
void cd_reg_win_rmw_cc_cd(uint32_t a0,uint32_t a1)
{
  cd_reg_window_rmw(a0,a1,0xcc,0xcd);
}

/* mc_op_result_poll — memcard op poll: cd_chan_job_submit ready-wait, then
 * cd_job_result samples; out@sp14 <5 uses JT @0x8012212c (idx 0,3 return
 * immediately; 1,2,4 dedup), >=5 dedup; returns last sample.
 * Disasm 0x80168424-0x801684c4. */
extern int32_t cd_chan_job_submit(uint32_t);
extern int32_t cd_job_result(uint32_t,uint32_t *,uint32_t *);
extern int br2_bail_pending(void);
int32_t mc_op_result_poll(uint32_t a0)
{
  uint32_t s0 = 0, sp10 = 0, sp14 = 0, r;
  for (;;) {
    while (cd_chan_job_submit(a0) == 0) {
      if (br2_bail_pending()) return 0;
      br2_irq_poll();
    }
    a0 = 0;
    while ((r = cd_job_result(a0,&sp10,&sp14)) == 0) {
      if (br2_bail_pending()) return 0;
      br2_irq_poll(); a0 = 0;
    }
    {
      uint32_t v1 = sp14;
      int imm = (v1 < 5 && (v1 == 0 || v1 == 3));
      if (!imm) {
        if (s0 != v1) { s0 = v1; continue; }
      }
      return (int32_t)v1;
    }
  }
}

/* mc_op_wait_stable — memcard op wait: poll cd_mc_op_wait until it returns 0
 * (->0) or the same nonzero value twice (->that value).
 * Disasm 0x801684c8-0x80168508. */
extern int32_t cd_mc_op_wait(uint32_t);
int32_t mc_op_wait_stable(uint32_t a0)
{
  uint32_t s0 = 0, r;
  for (;;) {
    r = (uint32_t)cd_mc_op_wait(a0);
    if (r == 0) return 0;
    if (s0 == r) return (int32_t)r;
    s0 = r;
  }
}

/* mc_chan_job_stable_poll — memcard op probe: poll cd_chan_job_pump(a0..a3,s2,s1)
 * until 0 (->0) or same nonzero twice (->that value).
 * Disasm 0x8016850c-0x801685ac. */
extern int32_t cd_chan_job_pump(uint32_t,const void *,const void *,const void *,uint32_t,uint32_t);
int32_t mc_chan_job_stable_poll(uint32_t a0,const void *a1,const void *a2,const void *a3,uint32_t s2,uint32_t s1)
{
  uint32_t s0 = 0, r;
  for (;;) {
    r = (uint32_t)cd_chan_job_pump(a0,a1,a2,a3,s2,s1);
    if (r == 0) return 0;
    if (s0 == r) return (int32_t)r;
    s0 = r;
  }
}

/* cd_range_table_member — range-table membership: a0 <<= shift@0x801bded8, scan
 * 8-int8_t entries @*(u32*)0x801bdf18; 0x80000000 -> skip, 0x40000000 ->
 * return 0; masked>=a0 or a0<masked+field4 -> return 1; else next.
 * Disasm 0x8019c060-0x8019c0e8. */
int32_t cd_range_table_member(uint32_t a0)
{
  /* guest: v0=*(0x801bded8); a0 <<= v0&31 (sllv); table=*(0x801bdf18);
   * if table==0 return 0 — the NULL test is on the CELL VALUE, not the
   * host pointer (br2eu_func_8019C060 @0x8019C064-0x8019C07C). The cell is
   * loaded once; the loop walks +8 without reloading it. */
  uint32_t sh = *(uint32_t *)guest_at(0x801bded8);
  uint32_t cell = *(uint32_t *)guest_at(0x801bdf18);
  uint8_t *p;
  uint32_t v1;
  if (cell == 0) return 0;
  p = (uint8_t *)guest_at(cell);
  a0 <<= (sh & 31);
  for (;;) {
    v1 = *(uint32_t *)p;
    if (v1 & 0x80000000) { p += 8; continue; }
    if (v1 & 0x40000000) return 0;
    v1 &= 0x0fffffff;
    if (v1 >= a0) return 1;
    if (a0 < v1 + *(uint32_t *)(p + 4)) return 1;
    p += 8;
  }
}

/* cd_status_bcc90 — return *(u32*)0x801bcc90.
 * Disasm 0x80199ffc-0x8019a008. */
int32_t cd_status_bcc90(void)
{
  return (int32_t)*(uint32_t *)guest_at(0x801bcc90);
}

/* cd_seek_param_issue_clamped — seek-param clamp+issue: a1 clamped to <=0x7eff0,
 * cd_seek_param_issue(a0,s0); if @0x801bdee8==0 clear @0x801bdee4; ret s0.
 * Disasm 0x8019d410-0x8019d468. */
extern uint32_t cd_seek_param_issue(uint32_t,uint32_t);
uint32_t cd_seek_param_issue_clamped(uint32_t a0,uint32_t a1)
{
  uint32_t s0 = a1;
  if (0x7eff0 < s0) s0 = 0x7eff0;
  cd_seek_param_issue(a0,s0);
  if (*(uint32_t *)guest_at(0x801bdee8) == 0)
    *(uint32_t *)guest_at(0x801bdee4) = 0;
  return s0;
}

/* cd_status_shifted_read — status read: if (a0-0x1010) <=u 0x7efe8 ->
 * sh = cd_reg_indexed_rw(-1) @0x801bdec8; return sh << shift@0x801bded8;
 * else 0. Disasm 0x8019d470-0x8019d4c8. */
extern int32_t cd_reg_indexed_rw(int32_t,uint32_t);
uint32_t cd_status_shifted_read(uint32_t a0)
{
  /* guest proceeds iff (a0-0x1010) <=u 0x7efe8 — sltu gives
   * v0=(0x7efe8 < diff) and branches to ret 0 when set
   * (br2eu_func_8019D470 @0x8019D484); the clean `< 0x7efe8` dropped the
   * diff==0x7efe8 boundary case. Final shl is sllv: amount &31. */
  if (a0 - 0x1010 <= 0x7efe8) {
    int32_t r = cd_reg_indexed_rw(-1,a0);
    uint32_t sh = (uint16_t)r;
    *(uint16_t *)guest_at(0x801bdec8) = (uint16_t)sh;
    sh = *(uint16_t *)guest_at(0x801bdec8);
    return sh << (*(uint32_t *)guest_at(0x801bded8) & 31);
  }
  return 0;
}

/* cd_mode_store — mode store: @0x801bde40=a0, @0x801bdecc=(a0==1)?1:0.
 * Disasm 0x8019d4d0-0x8019d500. */
void cd_mode_store(uint32_t a0)
{
  *(uint32_t *)guest_at(0x801bde40) = a0;
  *(uint32_t *)guest_at(0x801bdecc) = (a0 == 1) ? 1 : 0;
}

/* cd_seek_complete_poll — seek-complete state machine on @0x801bde40/@0x801bdee4:
 * early-out 1 when mode==1 or state==1; a0==1 waits for nonzero
 * cd_seek_state_machine(@0x801bde38) then stores @0x801bdee4; a0!=1 stores only
 * when result==1. Returns the sample. Disasm 0x8019d500-0x8019d5a0. */
extern int32_t cd_seek_state_machine(uint32_t);
/* cd_seek_complete_poll — faithful port of 0x8019d500-0x8019d5a0.
 * Early-outs (D51C/D530 -> D538 `li v0,1`): *0x801bde40==1 or
 * *0x801bdee4==1 return 1 without touching anything else.
 * Otherwise a0 = *0x801bde38 (event idx, reloaded before EVERY jal —
 * D544/D564) and the 0x8019d5b0 leaf tramp (`li t1,0x0B; jr 0xB0` =
 * kernel TestEvent) is called through cd_jal so $ra carries the real
 * guest link token (0x8019d550 first call / 0x8019d570 spin calls).
 *  - a0==1 (D558/D560/D570): ret!=0 -> D588 stores the DELAY-SLOT v0=1
 *    (D55C/D574 overwrite the raw TestEvent ret) to *0x801bdee4 and
 *    returns 1; ret==0 spins D560->D570. The spin crosses a block
 *    boundary per pass in generated code (psx_check_interrupts_at), so
 *    the clean port calls br2_irq_poll() each iteration — that is what
 *    lets the CD IRQ coroutine DeliverEvent the flag TestEvent polls.
 *  - a0!=1 (D580): ret!=1 -> D590 returns the RAW v0 (delay slot is a
 *    nop, v0 untouched); ret==1 -> D588 stores v0(==1) to *0x801bdee4
 *    then returns it.
 * No guest frame needed: the body only touches absolute cells
 * 0x801bde38/40/e4 and the BIOS tramp manages its own $sp. */
int32_t cd_seek_complete_poll(uint32_t a0)
{
  CPUState *cpu = clean_cpu_state();
  uint32_t v;

  if (DB32(0x801bde40) == 1) return 1;
  if (DB32(0x801bdee4) == 1) return 1;

  if (a0 == 1) {
    if (cpu) cpu->gpr[4] = DB32(0x801bde38);
    v = cd_jal(0x8019D5B0u, 0x8019D550u);
    if (v == 0) {
      for (;;) {
        if (br2_bail_pending()) return 0;
        br2_irq_poll();
        if (cpu) cpu->gpr[4] = DB32(0x801bde38);
        v = cd_jal(0x8019D5B0u, 0x8019D570u);
        if (v != 0) break;
      }
    }
    DB32(0x801bdee4) = 1;
    return 1;
  }

  if (cpu) cpu->gpr[4] = DB32(0x801bde38);
  v = cd_jal(0x8019D5B0u, 0x8019D550u);
  if (v == 1) DB32(0x801bdee4) = v;
  return (int32_t)v;
}

/* cd_dir_read — memcard dir read: mc_session(0) session, cmd 0x13
 * fetches BCD bounds into resp[1..2] (s0..s3), header record written,
 * then cmd 0x14 per index s0..s3 writes {resp[1],resp[2],0} entries.
 * Debug prints gated on @0x801bc850. Returns end-ptr (debug<2) or
 * entry count (debug>=2) — guest quirk preserved. Disasm 0x80195854-0x80195a74. */
uint32_t cd_dir_read(uint32_t a0,uint32_t a1)
{
  /* cmd/resp are handed to cd_cmd_issue_sync as raw guest addresses and
   * read/written by generated code — they must live in guest-visible
   * memory, so reserve a guest frame instead of using host stack. */
  uint32_t fr = br2_guest_frame(0x10);
  uint8_t *cmd = (uint8_t *)guest_at(fr);
  uint8_t *resp = (uint8_t *)guest_at(fr + 8);
  uint8_t *out = (uint8_t *)guest_at(a1);
  uint32_t s6, s0, s3, s2, dbg, i;
  uint8_t *s1p;
  (void)a0;
  cmd[0] = 1;
  s6 = mc_session(0);
  if (!cd_cmd_issue_sync(0x13,0,(uint32_t)(fr + 8))) goto fail;
  s0 = ((uint32_t)(resp[1] >> 4)) * 10 + (resp[1] & 0xf);
  s3 = ((uint32_t)(resp[2] >> 4)) * 10 + (resp[2] & 0xf);
  dbg = *(uint32_t *)guest_at(0x801bc850);
  if (dbg >= 2)
    printf_forward((const char *)guest_at(0x801255f8),s0,s3);
  cmd[0] = 0;
  if (!cd_cmd_issue_sync(0x14,(int)fr,(uint32_t)(fr + 8))) goto fail;
  out[0] = resp[1];
  out[2] = 0;
  out[1] = resp[2];
  if (s3 < s0) goto done;
  s2 = 1;
  s1p = out + 4;
  for (;;) {
    /* BCD encode s0 via /10 magic */
    uint32_t q = s0 / 10;
    cmd[0] = (uint8_t)(q * 16 + (s0 - q * 10));
    if (!cd_cmd_issue_sync(0x14,(int)fr,(uint32_t)(fr + 8))) goto fail;
    s2++;
    s1p[0] = resp[1];
    s1p[2] = 0;
    s1p[1] = resp[2];
    s0++;
    s1p += 4;
    if (s3 < s0) break;
  }
done:
  dbg = *(uint32_t *)guest_at(0x801bc850);
  if (dbg >= 2) {
    int32_t n = (int32_t)s2 - 1;
    if (n >= 0) {
      uint8_t *p = out;
      /* guest prints entries 0..n inclusive (n+1 = s2 entries: the header
       * plus every dir entry written above); bound is i <= n, verified
       * against br2eu_func_80195854 @0x80195A08 (beq on n<i exits). */
      for (i = 0; i <= (uint32_t)n; i++) {
        printf_forward((const char *)guest_at(0x80125608),p[0],p[1]);
        p += 4;
      }
    }
    mc_session(s6);
    br2_guest_frame_free();
    return (uint32_t)n;
  }
  mc_session(s6);
  br2_guest_frame_free();
  return guest_of(s1p);
fail:
  if (*(uint32_t *)guest_at(0x801bc850) != 0)
    printf_forward((const char *)guest_at(0x80125624));
  mc_session(s6);
  br2_guest_frame_free();
  return 0;
}

/* ======== batch31: CD register/block helpers (disasm-verified) ======== */

extern uint32_t cd_reg_window_op(uint32_t,uint32_t,uint32_t);
extern void cd_reg_stream_push(uint32_t,uint32_t);
extern void cd_delay_3c(void);
extern uint32_t cd_curve_lookup(uint32_t,uint32_t,uint32_t,uint32_t);
extern void cd_field_copy_gated(void *);
extern int32_t cd_mode_apply_deferred(uint32_t);
extern void cd_job_slot_alloc(uint32_t,uint32_t);
extern void cd_file_header_copy(uint32_t,void *);
extern void cd_strcat_guard(void *,uintptr_t);
extern void cd_pending_args_pump(void);
extern uint32_t cd_file_record_lookup(void *,void *);
extern uint32_t cd_queue_b_status(void);
extern uint32_t cd_status_map(uint32_t);
extern uint32_t cd_xchg_fec24(uint32_t);
extern uint32_t cd_chan_cmd_burst(uint32_t);
extern uint32_t cd_queue_a_read(void);
extern uint32_t cd_queue_a_status(void);

#define CDREG32(a) (*(uint32_t *)guest_at(a))
#define CDREG16(a) (*(uint16_t *)guest_at(a))

/* cd_seek_param_issue — seek/param issue: when mode flag @0x801bdecc==0 run
 * the 0x801feb70 register-window sequence via cd_reg_window_op, else
 * cd_reg_stream_push; returns a1 either way.
 * Disasm 0x8019bbc4-0x8019bc44. */
uint32_t cd_seek_param_issue(uint32_t a0,uint32_t a1)
{
  if (CDREG32(0x801bdecc) == 0) {
    uint32_t sh = (uint32_t)CDREG16(0x801bdec8)
                << (CDREG32(0x801bded8) & 31);   /* sllv: sa&31 */
    cd_reg_window_op(2,sh,0);
    cd_reg_window_op(1,sh,0);
    cd_reg_window_op(3,a0,a1);
  } else {
    cd_reg_stream_push(a0,a1);
  }
  return a1;
}

/* cd_reg_indexed_rw — indexed register write/read: optional round-up of a1
 * (divu guard @0x801bded4/@0x801bdedc, mask @0x801bdee0), shift right
 * by @0x801bded8; a0==-2 -> a1, a0==-1 -> (a3)&0xffff, else store a3
 * into u16 table @*(u32*)0x801bdeb0 + a0*2 and return -2.
 * NOTE: guest has `break 7` on divide-by-zero (unreachable guard);
 * ported as a plain `if (d)` guard.
 * Disasm 0x8019bcf0-0x8019bd90. */
int32_t cd_reg_indexed_rw(int32_t a0,uint32_t a1)
{
  uint32_t a3;
  if (CDREG32(0x801bded4) != 0) {
    uint32_t d = CDREG32(0x801bdedc);
    /* divu $zero,$a1,$d; on d==0 the guest models hi=a1 (dividend) and the
     * break7 trap is a recompiler no-op (br2eu_func_8019BCF0 @0x8019BD10-
     * 0x8019BD1C), so the round-up still runs with rem==a1. */
    uint32_t rem = (d != 0) ? (a1 % d) : a1;
    if (rem != 0)
      a1 = (a1 + d) & ~CDREG32(0x801bdee0);
  }
  a3 = a1 >> (CDREG32(0x801bded8) & 31);   /* srlv: sa&31 */
  if (a0 == -2) return (int32_t)a1;
  if (a0 == -1) return (int32_t)(a3 & 0xffff);
  CDMMIO16_ST(CDREG32(0x801bdeb0),(uint32_t)a0,(uint16_t)a3);
  return (int32_t)a1;                      /* v0=a1 set in delay slot */
}

/* cd_reg_window_u16 — paired u16 register window update. a1 &= 0x00ffffff,
 * hi/lo halves distributed to either the 0x801feb70 shadow regs
 * (flag @0x801bde9c&1) or the main table @*(u32*)0x801bdeb0
 * (+0x188/+0x18c for a0==1/a0==0) with set/clear of @0x801bde3c.
 * Disasm 0x8019d220-0x8019d3d8. */
void cd_reg_window_u16(int32_t a0,uint32_t a1)
{
  uint32_t a2, blk;
  uint16_t *r;
  a1 &= 0x00ffffff;
  a2 = a1 >> 16;
  if (CDREG32(0x801bde9c) & 1) {
    r = (uint16_t *)guest_at(0x801feb70);
    if (a0 == 1) {
      r[0] = (uint16_t)a1; r[1] = (uint16_t)a2;
      CDREG32(0x801bde68) |= 1;
      CDREG32(0x801bde64) |= a1;
      if (r[2] & a1) r[2] = (uint16_t)(r[2] & ~a1);
      if (r[3] & a2) r[3] = (uint16_t)(r[3] & ~a2);
      return;
    }
    if (a0 == 0) {
      r[2] = (uint16_t)a1; r[3] = (uint16_t)a2;
      CDREG32(0x801bde68) |= 1;
      CDREG32(0x801bde64) &= ~a1;         /* nor+and: clear mask */
      if (r[0] & a1) r[0] = (uint16_t)(r[0] & ~a1);
      if (r[1] & a2) r[1] = (uint16_t)(r[1] & ~a2);
    }
    return;
  }
  blk = CDREG32(0x801bdeb0);
  if (a0 == 1) {
    CDMMIO16_ST(blk,0xc4,(uint16_t)a1);    /* +0x188 */
    CDMMIO16_ST(blk,0xc5,(uint16_t)a2);    /* +0x18a */
    CDREG32(0x801bde3c) |= a1;
  } else if (a0 == 0) {
    CDMMIO16_ST(blk,0xc6,(uint16_t)a1);    /* +0x18c */
    CDMMIO16_ST(blk,0xc7,(uint16_t)a2);    /* +0x18e */
    CDREG32(0x801bde3c) &= ~a1;
  }
}

/* cd_chan_attr_apply — channel-attribute block apply: a0 = guest ptr to
 * {u32 flags, u16 vals...}. flags==0 (t2) applies everything; else
 * each field gated by flag bits. Two indexed channels (+0x180/+0x182)
 * use 7-entry magnitude tables {0x8000..0xe000} (payload @0x80125c68/
 * 0x80125c88), then per-bit stores into the 0x10-stride entry table
 * @*(u32*)0x801bdeb0 plus bit ops on +0x1aa.
 * Disasm 0x8019d5f0-0x8019d968. */
void cd_chan_attr_apply(uint32_t a0)
{
  static const uint32_t jt_mag[8] = {
    0,0x8000,0x9000,0xa000,0xb000,0xc000,0xd000,0xe000 };
  uint8_t *p = (uint8_t *)guest_at(a0);
  uint32_t blk;
  uint32_t t1 = *(uint32_t *)p;
  int t2 = (t1 < 1u);
  uint32_t a1, a2, v0;
  int16_t idx, sv;
  uint32_t lu;

  /* ---- channel 1 -> +0x180 ---- */
  if (t2 || (t1 & 1)) {
    a1 = 0; a2 = 0;
    if (t2 || (t1 & 4)) {
      idx = *(int16_t *)(p + 8);
      if (idx >= 1 && idx < 8) {
        a1 = jt_mag[idx];
        sv = *(int16_t *)(p + 4); lu = *(uint16_t *)(p + 4);
        /* guest: sv<0 -> a2 stays 0x7f (blt @0x8019D6A4 -> 0x8019D6B0),
         * NOT 0 — the old (sv<0)?0:lu diverged (br2eu_func_8019D5F0). */
        a2 = 0x7f;
        if (sv < 0x80) a2 = (sv < 0) ? 0x7f : lu;
      } else {
        a2 = *(uint16_t *)(p + 4);
      }
    } else {
      a2 = *(uint16_t *)(p + 4);
    }
    v0 = a2 & 0x7fff;
    blk = CDREG32(0x801bdeb0);
    CDMMIO16_ST(blk,0xc0,(uint16_t)(v0 | a1));
  }

  /* ---- channel 2 -> +0x182 ---- */
  if (t2 || (t1 & 2)) {
    a1 = 0; a2 = 0;
    if (t2 || (t1 & 8)) {
      idx = *(int16_t *)(p + 0xa);
      if (idx >= 1 && idx < 8) {
        a1 = jt_mag[idx];
        sv = *(int16_t *)(p + 6); lu = *(uint16_t *)(p + 6);
        a2 = 0x7f;
        if (sv < 0x80) a2 = (sv < 0) ? 0x7f : lu;   /* same sv<0 fix */
      } else {
        a2 = *(uint16_t *)(p + 6);
      }
    } else {
      a2 = *(uint16_t *)(p + 6);
    }
    v0 = a2 & 0x7fff;
    blk = CDREG32(0x801bdeb0);
    CDMMIO16_ST(blk,0xc1,(uint16_t)(v0 | a1));
  }

  /* ---- simple u16 field copies ---- */
  if (t2 || (t1 & 0x40)) {
    blk = CDREG32(0x801bdeb0);
    CDMMIO16_ST(blk,0xd8,*(uint16_t *)(p + 0x10));
  }
  if (t2 || (t1 & 0x80)) {
    blk = CDREG32(0x801bdeb0);
    CDMMIO16_ST(blk,0xd9,*(uint16_t *)(p + 0x12));
  }
  if (t2 || (t1 & 0x400)) {
    blk = CDREG32(0x801bdeb0);
    CDMMIO16_ST(blk,0xda,*(uint16_t *)(p + 0x1c));
  }
  if (t2 || (t1 & 0x800)) {
    blk = CDREG32(0x801bdeb0);
    CDMMIO16_ST(blk,0xdb,*(uint16_t *)(p + 0x1e));
  }

  /* ---- +0x1aa bit ops driven by u32 fields ---- */
  if (t2 || (t1 & 0x100)) {
    blk = CDREG32(0x801bdeb0);
    if (*(uint32_t *)(p + 0x14) == 0) CDMMIO16_ST(blk,0xd5,(uint16_t)(CDMMIO16(blk,0xd5) & ~4u));
    else                            CDMMIO16_ST(blk,0xd5,(uint16_t)(CDMMIO16(blk,0xd5) | 4));
  }
  if (t2 || (t1 & 0x200)) {
    blk = CDREG32(0x801bdeb0);
    if (*(uint32_t *)(p + 0x18) == 0) CDMMIO16_ST(blk,0xd5,(uint16_t)(CDMMIO16(blk,0xd5) & ~1u));
    else                            CDMMIO16_ST(blk,0xd5,(uint16_t)(CDMMIO16(blk,0xd5) | 1));
  }
  if (t2 || (t1 & 0x1000)) {
    blk = CDREG32(0x801bdeb0);
    if (*(uint32_t *)(p + 0x20) == 0) CDMMIO16_ST(blk,0xd5,(uint16_t)(CDMMIO16(blk,0xd5) & ~8u));
    else                            CDMMIO16_ST(blk,0xd5,(uint16_t)(CDMMIO16(blk,0xd5) | 8));
  }
  if (t2 || (t1 & 0x2000)) {
    blk = CDREG32(0x801bdeb0);
    if (*(uint32_t *)(p + 0x24) == 0) CDMMIO16_ST(blk,0xd5,(uint16_t)(CDMMIO16(blk,0xd5) & ~2u));
    else                            CDMMIO16_ST(blk,0xd5,(uint16_t)(CDMMIO16(blk,0xd5) | 2));
  }
}

/* cd_chan_attr_push — per-channel attribute push: loops s4=0..0x17, gated
 * per iteration by bit s4 of *(u32*)a0 and per-field by flags @a0+4
 * (s1==0 => apply all). Entries are 0x10-int8_t stride in the table
 * @*(u32*)0x801bdeb0; scratch u16s at 0x801bde6c+s4*2.
 * Magnitude sub-tables: payload @0x80125ca8 / @0x80125cc8 ->
 * {0x8000..0xe000} for mode-1 in 0..6.
 * Disasm 0x8019dad0-0x8019e0c4. */
void cd_chan_attr_push(uint32_t a0)
{
  static const uint32_t jt_dad[7] = {
    0x8000,0x9000,0xa000,0xb000,0xc000,0xd000,0xe000 };
  uint8_t *p = (uint8_t *)guest_at(a0);
  uint32_t E;
  uint16_t *S;
  uint32_t s1 = *(uint32_t *)(p + 4);
  int s2 = (s1 < 1u);
  int s4;
  uint32_t a1, a2, v0, v1;

  for (s4 = 0; s4 < 0x18; s4++) {
    if ((*(uint32_t *)p & (1u << s4)) == 0) continue;
    {
      uint32_t s3 = (uint32_t)s4 << 3;
      E = CDREG32(0x801bdeb0) + s4 * 0x10;
      S = (uint16_t *)guest_at(0x801bde6c + s4 * 2);

      /* +0x10 mask: entry[2] = p.u16[+0x14] */
      if (s2 || (s1 & 0x10)) CDMMIO16_ST(E,2,*(uint16_t *)(p + 0x14));

      /* +0x40 mask: scratch = p.u16[+0x18] */
      if (s2 || (s1 & 0x40)) *S = *(uint16_t *)(p + 0x18);

      /* +0x20 mask: entry[2] = cd_curve_lookup(int8_t split of scratch,
       * int8_t split of p.u16[+0x16]) */
      if (s2 || (s1 & 0x20)) {
        uint16_t sv = *S, pv = *(uint16_t *)(p + 0x16);
        uint32_t r = (uint32_t)cd_curve_lookup(sv >> 8, sv & 0xff,
                                             pv >> 8, pv & 0xff);
        CDMMIO16_ST(E,2,(uint16_t)r);
      }

      /* +0x1 mask: channel A -> CDMMIO16(E,0) */
      if (s2 || (s1 & 1)) {
        uint32_t mc = 0;
        a1 = *(uint16_t *)(p + 8) & 0x7fff;
        if (s2 || (s1 & 4)) {
          uint32_t m = (uint32_t)(int32_t)(int16_t)(*(uint16_t *)(p + 0xc) - 1);
          if (m < 7) mc = jt_dad[m];
        }
        if (mc != 0) {
          int16_t w = *(int16_t *)(p + 8);
          if (w >= 0x80) a1 = 0x7f;
          else if (w < 0) a1 = 0;
        }
        CDMMIO16_ST(E,0,(uint16_t)(a1 | mc));
      }

      /* +0x2 mask: channel B -> CDMMIO16(E,1) */
      if (s2 || (s1 & 2)) {
        uint32_t mc = 0;
        a1 = *(uint16_t *)(p + 0xa) & 0x7fff;
        if (s2 || (s1 & 8)) {
          uint32_t m = (uint32_t)(int32_t)(int16_t)(*(uint16_t *)(p + 0xe) - 1);
          if (m < 7) mc = jt_dad[m];
        }
        if (mc != 0) {
          int16_t w = *(int16_t *)(p + 0xa);
          if (w >= 0x80) a1 = 0x7f;
          else if (w < 0) a1 = 0;
        }
        CDMMIO16_ST(E,1,(uint16_t)(a1 | mc));
      }

      /* +0x80 mask: cd_reg_indexed_rw(s3|3, p.u32[+0x1c]) */
      if (s2 || (s1 & 0x80))
        cd_reg_indexed_rw((int32_t)(s3 | 3), *(uint32_t *)(p + 0x1c));

      /* +0x1 mask again: cd_reg_indexed_rw(s3|7, p.u32[+0x20]) */
      if (s2 || (s1 & 1))
        cd_reg_indexed_rw((int32_t)(s3 | 7), *(uint32_t *)(p + 0x20));

      /* +0x2 mask: CDMMIO16_ST(E,4,p.u16[+0x3a] */
      if (s2 || (s1 & 2)) CDMMIO16_ST(E,4,*(uint16_t *)(p + 0x3a));

      /* +0x4 mask: CDMMIO16_ST(E,5,p.u16[+0x3c] */
      if (s2 || (s1 & 4)) CDMMIO16_ST(E,5,*(uint16_t *)(p + 0x3c));

      /* +0x800 mask: CDMMIO16(E,4) hi int8_t fields */
      if (s2 || (s1 & 0x800)) {
        a1 = *(uint16_t *)(p + 0x30);
        if (a1 >= 0x80) a1 = 0x7f;
        a2 = 0;
        if (s2 || (s1 & 0x100)) {
          if (*(uint32_t *)(p + 0x24) == 5) a2 = 0x80;
        }
        v0 = CDMMIO16(E,4);
        CDMMIO16_ST(E,4,(uint16_t)((v0 & 0xff) | ((a1 | a2) << 8)));
      }

      /* +0x1000 mask: CDMMIO16(E,4) bits 4-7 */
      if (s2 || (s1 & 0x1000)) {
        a1 = *(uint16_t *)(p + 0x32);
        if (a1 >= 0x10) a1 = 0xf;
        v0 = CDMMIO16(E,4);
        CDMMIO16_ST(E,4,(uint16_t)((v0 & 0xff0f) | (a1 << 4)));
      }

      /* +0x2000 mask: CDMMIO16(E,5) bits 6-15 */
      if (s2 || (s1 & 0x2000)) {
        a1 = *(uint16_t *)(p + 0x34);
        if (a1 >= 0x80) a1 = 0x7f;
        a2 = 0x100;
        if (s2 || (s1 & 0x200)) {
          v1 = *(uint32_t *)(p + 0x28);
          if (v1 == 5) a2 = 0x200;
          else if (v1 < 6) { if (v1 == 1) a2 = 0; }
          else if (v1 == 7) a2 = 0x300;
        }
        v0 = CDMMIO16(E,5);
        CDMMIO16_ST(E,5,(uint16_t)((v0 & 0x3f) | ((a1 | a2) << 6)));
      }

      /* +0x4000 mask: CDMMIO16(E,5) bits 5-9 */
      if (s2 || (s1 & 0x4000)) {
        a1 = *(uint16_t *)(p + 0x36);
        if (a1 >= 0x20) a1 = 0x1f;
        a2 = 0;
        if (s2 || (s1 & 0x400)) {
          v1 = *(uint32_t *)(p + 0x2c);
          if (v1 == 7) a2 = 0x20;   /* v1==3 also lands here with a2=0 */
        }
        v0 = CDMMIO16(E,5);
        CDMMIO16_ST(E,5,(uint16_t)((v0 & 0xffc0) | (a1 | a2)));
      }

      /* +0x8000 mask: CDMMIO16(E,4) low nibble */
      if (s2 || (s1 & 0x8000)) {
        a1 = *(uint16_t *)(p + 0x38);
        if (a1 >= 0x10) a1 = 0xf;
        v0 = CDMMIO16(E,4);
        CDMMIO16_ST(E,4,(uint16_t)((v0 & 0xfff0) | a1));
      }
    }
  }
}

/* cd_chan_job_submit — channel-job submit: state block @0x801febc0 must be
 * idle (<=0); stores mode 2 + arg, kicks cd_job_slot_alloc, returns 1;
 * busy -> printf + 0.
 * Disasm 0x8019ea98-0x8019eafc. */
int32_t cd_chan_job_submit(uint32_t a0)
{
  uint32_t *st = (uint32_t *)guest_at(0x801febe0);
  if ((int32_t)st[0] > 0) {
    printf_forward((const char *)guest_at(0x80125ce8));
    return 0;
  }
  st[0] = 2;
  st[1] = 0;
  st[2] = 0;
  st[4] = a0;                        /* delay-slot store: before call */
  cd_job_slot_alloc(0x8019eb00,a0);       /* a1 = orig a0 (delay-slot move) */
  return 1;
}

/* cd_job_result — channel-job result sample: reads state @0x801febc0.
 * a0==0: drains first nonzero slot (busy-polls +8/+0x10/+0x18... per
 * guest), out a2<-@0x801febd4, a1<-@0x801febd0, clears +8, ret 1.
 * a0!=0: empty -> a2<-+4, a1<-+0 (ret 0); pending -> a2<-@0x801febd4,
 * a1<-@0x801febd0, clears +8, ret 1; totally idle -> -1.
 * a1/a2 are HOST out-pointers (callers pass &local).
 * Disasm 0x8019fa80-0x8019fb98. */
int32_t cd_job_result(uint32_t a0,uint32_t *a1,uint32_t *a2)
{
  uint32_t *st = (uint32_t *)guest_at(0x801febe0);
  uint32_t t0, a3;
  if (st[0] == 0) {
    if (st[2] == 0) return -1;
  }
  t0 = st[0];
  a3 = st[1];
  if (a0 == 0) {
    uint32_t *v = st;
    if (v[2] == 0) {
      v += 2;                          /* +8 slot scan (guest quirk:
                                        * polls single next slot) */
      while (*v == 0) {
        if (br2_bail_pending()) return 0;
        br2_irq_poll();
      }
    }
    if (a2) *a2 = CDREG32(0x801febd4);
    if (a1) *a1 = CDREG32(0x801febd0);
    st[2] = 0;
    return 1;
  }
  if (st[2] == 0) {
    if (a2) *a2 = a3;
    if (a1 == 0) return 0;
    *a1 = t0;
    return 0;
  }
  if (a2) *a2 = CDREG32(0x801febd4);
  if (a1) *a1 = CDREG32(0x801febd0);
  st[2] = 0;
  return 1;
}

/* cd_mc_op_wait — channel-job wait: busy-check, cd_chan_cmd_burst gate,
 * retry while cd_queue_b_status()&4; success path waits cd_queue_a_read then

 * returns cd_status_map(cd_queue_a_status()).
 * Disasm 0x8019ff4c-0x801a0004. */
int32_t cd_mc_op_wait(uint32_t a0)
{
  uint32_t s0 = a0;
  uint32_t v0;
  if (CDREG32(0x801febe0) != 0) {
    printf_forward((const char *)guest_at(0x80125e8c));
    return -1;
  }
retry:
  cd_pending_args_pump();
  if (cd_chan_cmd_burst(s0) != 0) {
    uint32_t r;
    cd_pending_args_pump();
    bios_tramp_a0ac(CDREG32(0x801febf0),0,0,0);
    do {
      if (br2_bail_pending()) return -1;
      br2_irq_poll();
    } while (cd_queue_a_read() == 0);
    r = cd_queue_a_status();
    return (int32_t)cd_status_map(r);
  }
  v0 = cd_queue_b_status();
  if ((v0 & 4) != 0) goto retry;
  return (int32_t)cd_status_map(v0);
}

/* cd_chan_job_pump — channel-job pump: submits blk/rec pairs through the
 * 0x801febc0 job channel until fp+a5 records produced; copies 0x28-int8_t
 * records to a2 once s1 >= fp (signed). a1/a2/a3 are HOST pointers
 * (callers pass stack/arena buffers). Returns last error or 0;
 * writes produced count to *a3.
 * Disasm 0x8019f80c-0x8019fa68. */
int32_t cd_chan_job_pump(uint32_t a0,const void *a1,const void *a2,const void *a3,
                     uint32_t a4,uint32_t a5)
{
  /* blk/rec mirror the guest frame (sp+0x10 / sp+0x30 of a 0x60 frame)
   * but must live in guest RAM: they cross the BIOS-service boundary
   * (svc B0/0x42 tail call in cd_file_record_lookup) and host stack
   * buffers are invisible to guest code. br2_guest_frame reserves real
   * guest stack below $sp so nested BIOS frames can't collide. */
  uint32_t fr = br2_guest_frame(0x60);
  uint32_t *blk = (uint32_t *)guest_at(fr + 0x10);   /* sp+0x10: 0x20 job blk */
  uint32_t *rec = (uint32_t *)guest_at(fr + 0x30);   /* sp+0x30: 0x28 record  */
  uint32_t err = 0;
  uint32_t s0, s1, s2, s4, s5;
  uint32_t fp = a4, lim = a5;
  uintptr_t a1v;
  uint32_t st0;
  uint8_t *dsta;
  int32_t v0;
  uint32_t *outa = (uint32_t *)a3;

  if (CDREG32(0x801febe0) != 0) {
    printf_forward((const char *)guest_at(0x80125e8c));
    br2_guest_frame_free();
    return -1;
  }
  a1v = (uintptr_t)a1;
  cd_file_header_copy(a0,blk);
  cd_strcat_guard(blk,a1v);
  s2 = 0; s1 = 0;
  CDREG32(0x801febec) |= 1u << (CDREG32(0x801febf0) & 31);  /* sllv */
  v0 = (int32_t)(fp + lim);
  if (v0 <= 0) goto done;
  s5 = 0; s4 = 0;
  dsta = (uint8_t *)a2;
  for (;;) {
    if (s1 == 0) {
      cd_pending_args_pump();
      s0 = cd_file_record_lookup(blk,rec);
      if (s0 == 0) {
        uint32_t r;
        r = cd_status_map(cd_queue_b_status());
        err = r;                        /* sw v0,0x58(sp): always */
        if (r == 0) goto done;
        s2++;
        if (s2 < 4) continue;
        /* 4 strikes: teardown + drain + report last error */
        v0 = (int32_t)cd_xchg_fec24(0);
        CDREG32(0x801fec38) = (uint32_t)v0;
        st0 = CDREG32(0x801febe0);
        if ((int32_t)st0 > 0) {
          printf_forward((const char *)guest_at(0x80125ce8));
        } else {
          uint32_t *st = (uint32_t *)guest_at(0x801febe0);
          st[0] = 2; st[1] = 0; st[2] = 0;
          CDREG32(0x801febf0) = a0;      /* delay-slot store before call */
          cd_job_slot_alloc(0x8019eb00,(uint32_t)a1v);
        }
        cd_job_result(0,0,&err);
        cd_xchg_fec24(CDREG32(0x801fec38));
        br2_guest_frame_free();
        return (int32_t)err;
      }
    } else {
      s0 = bios_tramp_b043(guest_of(rec),0,0,0);
      if (s0 == 0) goto done;
    }
    /* f9b0: copy only once s1 >= fp (signed) and a2 != 0 */
    if (!((int32_t)s1 < (int32_t)fp) && dsta != NULL) {
      memcpy(dsta + s4,rec,0x28);
      s4 += 0x28;
      s5++;
      a1v = rec[7];                    /* last lw a1 in copy loop */
      (void)a1v;
    }
    s1++;
    if ((int32_t)s1 < (int32_t)(fp + lim)) continue;
done:
    break;
  }
  if (outa) *outa = s5;
  br2_guest_frame_free();
  return 0;
}

/* cd_chan_reg_init — CD channel/register init: sets flag bits via
 * @0x801bdec0 ptr, clears mode regs, waits for channel idle
 * (+0x1ae & 0x7ff, max 0xf01 polls), programs shift/mode table
 * @0x801bded4-dc/e0, zeroes 10 u16 @0x801feb70, optional full
 * channel-block init when arg==0, then arms @0x801bdee4/8/ec.
 * Disasm 0x8019b3a0-0x8019b61c. */
void cd_chan_reg_init(int32_t a0)
{
  uint32_t blk;
  uint32_t i, v1;
  int s0 = a0;

  {
    /* DPCR |= 0xB0000 — enables the channel-4 (SPU stream) DMA. The
     * pointer cell resolves to 0x1F8010F0 (MMIO); must RMW through
     * br2 accessors, not guest_at (open-bus mirror). */
    uint32_t dpcr = CDREG32(0x801bdec0);
    br2_st32(dpcr, br2_ld32(dpcr) | 0xb0000u);
  }
  CDREG32(0x801bdecc) = 0;
  CDREG32(0x801bddd0) = 0;
  CDREG16(0x801bdec8) = 0;
  blk = CDREG32(0x801bdeb0);
  CDMMIO16_ST(blk,0xc0,0); CDMMIO16_ST(blk,0xc1,0); CDMMIO16_ST(blk,0xd5,0);
  cd_delay_3c();
  blk = CDREG32(0x801bdeb0);
  CDMMIO16_ST(blk,0xc0,0); CDMMIO16_ST(blk,0xc1,0);
  v1 = 0;
  if ((CDMMIO16(blk,0xd7) & 0x7ff) != 0) {
    for (;;) {
      v1++;
      if (v1 >= 0xf01) {
        printf_forward((const char *)guest_at(0x80125be8),
                     (const char *)guest_at(0x80125bf8));
        break;
      }
      blk = CDREG32(0x801bdeb0);
      if ((CDMMIO16(blk,0xd7) & 0x7ff) == 0) break;
    }
  }
  CDREG32(0x801bded4) = 2;
  CDREG32(0x801bded8) = 3;
  CDREG32(0x801bdedc) = 8;
  CDREG32(0x801bdee0) = 7;
  blk = CDREG32(0x801bdeb0);
  CDMMIO16_ST(blk,0xd6,4);                            /* +0x1ac */
  CDMMIO16_ST(blk,0xc2,0); CDMMIO16_ST(blk,0xc3,0);               /* +0x184,+0x186 */
  CDMMIO16_ST(blk,0xc6,0xffff); CDMMIO16_ST(blk,0xc7,0xffff);     /* +0x18c,+0x18e */
  CDMMIO16_ST(blk,0xcc,0); CDMMIO16_ST(blk,0xcd,0);               /* +0x198,+0x19a */
  {
    uint16_t *z = (uint16_t *)guest_at(0x801feb70);
    for (i = 0; i < 10; i++) z[i] = 0;
  }
  if (s0 != 0) goto tail;

  /* ---- arg==0: full channel-block init ---- */
  CDREG16(0x801bdec8) = 0x200;
  blk = CDREG32(0x801bdeb0);
  CDMMIO16_ST(blk,0xc8,0); CDMMIO16_ST(blk,0xc9,0); CDMMIO16_ST(blk,0xca,0); CDMMIO16_ST(blk,0xcb,0);
  CDMMIO16_ST(blk,0xd8,0); CDMMIO16_ST(blk,0xd9,0); CDMMIO16_ST(blk,0xda,0); CDMMIO16_ST(blk,0xdb,0);
  cd_reg_stream_push(0x801bdef0,0x10);
  blk = CDREG32(0x801bdeb0);
  for (i = 0; i < 0x18; i++) {
    uint32_t e = blk + i * 0x10;          /* 0x10-int8_t stride */
    CDMMIO16_ST(e,0,0); CDMMIO16_ST(e,1,0); CDMMIO16_ST(e,2,0x3fff);
    CDMMIO16_ST(e,3,0x200); CDMMIO16_ST(e,4,0); CDMMIO16_ST(e,5,0);
  }
  blk = CDREG32(0x801bdeb0);
  CDMMIO16_ST(blk,0xc4,0xffff); CDMMIO16_ST(blk,0xc5,0xff);       /* +0x188,+0x18a */
  cd_delay_3c(); cd_delay_3c(); cd_delay_3c(); cd_delay_3c();
  blk = CDREG32(0x801bdeb0);
  CDMMIO16_ST(blk,0xc6,0xffff); CDMMIO16_ST(blk,0xc7,0xff);       /* +0x18c,+0x18e */
  cd_delay_3c(); cd_delay_3c(); cd_delay_3c(); cd_delay_3c();

tail:
  blk = CDREG32(0x801bdeb0);
  CDREG32(0x801bdee4) = 1;
  CDMMIO16_ST(blk,0xd5,0xc000);                       /* +0x1aa */
  CDREG32(0x801bdee8) = 0;
  CDREG32(0x801bdeec) = 0;
}

/* cd_reg_window_rmw — register-window RMW: op a0 (0=AND-NOT,1=OR,8=STORE,
 * else passthrough), value a1 (lo16 -> index a2, bits16-23 -> index
 * a3 of the u16 reg file). Register file base = 0x801fe9e8 when
 * @0x801bde9c&1 else @*(u32*)0x801bdeb0; alt path also sets bit
 * (a2-0xc6)>>1 in @0x801bde68. Returns combined old/new fields.
 * Disasm 0x8019cb50-0x8019ce08. */
uint32_t cd_reg_window_rmw(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3)
{
  uint32_t t1 = a0, t0 = a1;
  int alt = (int)(CDREG32(0x801bde9c) & 1);
  /* base is a guest ADDRESS (RAM shadow 0x801fe9e8 or the MMIO reg
   * block); br2_ld16/st16 route each access to RAM or the device. */
  uint32_t base = alt ? 0x801fe9e8u : CDREG32(0x801bdeb0);
  uint32_t v1 = br2_ld16(base + (a3 << 1)) & 0xff;
  uint32_t v0 = br2_ld16(base + (a2 << 1));
  uint32_t t2 = (v1 << 16) | v0;
  uint32_t hb = (t0 >> 16) & 0xff;
  int32_t sh;

  if (t1 == 1) {                            /* OR */
    if (alt) {
      br2_st16(base + (a2 << 1), (uint16_t)(br2_ld16(base + (a2 << 1)) | t0));
      br2_st16(base + (a3 << 1), (uint16_t)(br2_ld16(base + (a3 << 1)) | hb));
      sh = ((int32_t)(a2 - 0xc6)) >> 1;
      CDREG32(0x801bde68) |= 1u << (sh & 31);
    } else {
      br2_st16(base + (a2 << 1), (uint16_t)(br2_ld16(base + (a2 << 1)) | t0));
      br2_st16(base + (a3 << 1), (uint16_t)(br2_ld16(base + (a3 << 1)) | hb));
    }
    t2 |= t0;
    return t2;
  }
  if (t1 < 2) {                            /* AND-NOT (t1==0) */
    if (alt) {
      br2_st16(base + (a2 << 1), (uint16_t)(br2_ld16(base + (a2 << 1)) & ~t0));
      br2_st16(base + (a3 << 1), (uint16_t)(br2_ld16(base + (a3 << 1)) & ~(uint32_t)hb));
      sh = ((int32_t)(a2 - 0xc6)) >> 1;
      CDREG32(0x801bde68) |= 1u << (sh & 31);
    } else {
      br2_st16(base + (a2 << 1), (uint16_t)(br2_ld16(base + (a2 << 1)) & ~t0));
      br2_st16(base + (a3 << 1), (uint16_t)(br2_ld16(base + (a3 << 1)) & ~(uint32_t)hb));
    }
    return t2 & ~t0;
  }
  if (t1 == 8) {                            /* STORE */
    if (alt) {
      br2_st16(base + (a2 << 1), (uint16_t)t0);
      br2_st16(base + (a3 << 1), (uint16_t)hb);
      sh = ((int32_t)(a2 - 0xc6)) >> 1;
      CDREG32(0x801bde68) |= 1u << (sh & 31);
    } else {
      br2_st16(base + (a2 << 1), (uint16_t)t0);
      br2_st16(base + (a3 << 1), (uint16_t)hb);
    }
    return t0;
  }
  return t2;
}

/* cd_chan_mode_apply — channel mode/param apply. a0 = guest ptr to
 * {u32 f0, u32 f1, u16 f2@8, u16 f3@0xa, u32 f4@0xc, u32 f5@0x10}.
 * Mode name table @0x801be470 + mode*0x44 (0x44-int8_t records);
 * function table @0x801be440. Returns -1 on bad mode/range check,
 * else 0. Disasm 0x8019c0f0-0x8019c5c0. */
int32_t cd_chan_mode_apply(uint32_t a0)
{
  uint8_t *blk = (uint8_t *)guest_at(a0);
  uint8_t buf[0x44];
  uint16_t *w = (uint16_t *)buf;
  uint32_t s3, s0, s5, s4 = 0, s6 = 0, s7 = 0, fp = 0, f58 = 0;
  uint32_t mode, vv, ent;
  int32_t i;
  uint8_t *src;
  int32_t x13, x12, hi1, hi2, q1, q2, q, xs;
  uint32_t xs_u, p_hi;
  int64_t prod;

  memset(buf,0,sizeof buf);
  s3 = *(uint32_t *)blk;
  s5 = (s3 < 1u);

  /* ---- block 1: mode dispatch (s3==0 or s3&1) ---- */
  if (s5 || (s3 & 1)) {
    s0 = *(uint32_t *)(blk + 4);
    if (s0 & 0x100) { s0 &= ~0x100u; f58 = 1; }
    if (s0 < 0xa) {
      uint32_t *tab = (uint32_t *)guest_at(0x801be440);
      ent = tab[s0];
      s4 = 1;
      if (cd_range_table_member(ent) != 0) return -1;
      mode = s0;
      CDREG32(0x801bde54) = mode;
      mode = CDREG32(0x801bde54);
      CDREG32(0x801bde4c) = ((uint32_t *)guest_at(0x801be440))[mode];
      src = (uint8_t *)guest_at(0x801be470 + mode * 0x44);
      memcpy(buf,src,0x44);
      if (mode == 7) {
        CDREG32(0x801bde60) = 0x7f;
        CDREG32(0x801bde5c) = 0x7f;
      } else if (mode == 8) {
        CDREG32(0x801bde60) = 0;
        CDREG32(0x801bde5c) = 0x7f;
      } else {
        CDREG32(0x801bde60) = 0;
        CDREG32(0x801bde5c) = 0;
      }
    } else {
      return -1;
    }
  }

  /* ---- block 2: rate math (s3==0 or s3&8), mode in {7,8} ---- */
  if (s5 || (s3 & 8)) {
    mode = CDREG32(0x801bde54);
    if (mode >= 7 && mode < 9) {
      s6 = 1;
      if (!s4) {
        src = (uint8_t *)guest_at(0x801be470 + mode * 0x44);
        memcpy(buf,src,0x44);
        *(uint32_t *)buf = 0xc01c1c00;
      }
      vv = *(uint32_t *)(blk + 0xc);
      x13 = (int32_t)(vv << 13);
      x12 = (int32_t)(vv << 12);
      prod = (int64_t)x13 * (int64_t)0x8102409;
      hi1 = (int32_t)(prod >> 32);
      prod = (int64_t)x12 * (int64_t)0x8102409;
      hi2 = (int32_t)(prod >> 32);
      CDREG32(0x801bde5c) = vv;
      xs_u = (uint32_t)hi1 + (uint32_t)x13;
      q1 = ((int32_t)xs_u >> 6) - (x13 >> 31);
      xs_u = (uint32_t)hi2 + (uint32_t)x12;
      q2 = ((int32_t)xs_u >> 6) - (x12 >> 31);
      w[0x0c] = (uint16_t)(q1 - w[0x02]);       /* sp+0x28 <- q1 - u16@0x14 */
      w[0x0d] = (uint16_t)(q2 - w[0x03]);       /* sp+0x2a <- q2 - u16@0x16 */
      w[0x12] = (uint16_t)(w[0x13] + q2);       /* sp+0x34 <- u16@0x36 + q2 */
      w[0x0e] = (uint16_t)(w[0x0f] + q2);       /* sp+0x2c <- u16@0x2e + q2 */
      w[0x1d] = (uint16_t)(w[0x1f] + q2);       /* sp+0x4a <- u16@0x4e + q2 */
      w[0x1c] = (uint16_t)(w[0x1e] + q2);       /* sp+0x48 <- u16@0x4c + q2 */
    }
  }

  /* ---- block 3: secondary rate (s3==0 or s3&0x10), mode {7,8} ---- */
  if (s5 || (s3 & 0x10)) {
    mode = CDREG32(0x801bde54);
    if (mode >= 7 && mode < 9) {
      fp = 1;                             /* delay slot: always */
      if (!s4) {
        if (!s6) {
          src = (uint8_t *)guest_at(0x801be470 + mode * 0x44);
          memcpy(buf,src,0x44);
          *(uint32_t *)buf = 0x80;
        } else {
          *(uint32_t *)buf |= 0x80;
        }
      }
      vv = *(uint32_t *)(blk + 0x10);
      xs_u = ((vv << 7) + vv) << 8;
      xs = (int32_t)xs_u;
      prod = (int64_t)xs * (int64_t)0x8102409;
      p_hi = (uint32_t)(int32_t)(prod >> 32);
      xs_u = p_hi + xs_u;
      q = ((int32_t)xs_u >> 6) - (xs >> 31);
      CDREG32(0x801bde60) = vv;
      w[0x09] = (uint16_t)q;                  /* sp+0x22 */
    }
  }

  /* ---- s4: mode-active path clears fields; else flag stores ---- */
  {
    uint32_t gmb = CDREG32(0x801bdeb0);
    if (s4 != 0) {
      s7 = (CDMMIO16(gmb,0xd5) >> 7) & 1;
      if (s7) CDMMIO16_ST(gmb,0xd5,(uint16_t)(CDMMIO16(gmb,0xd5) & 0xff7f));
      CDMMIO16_ST(gmb,0xc2,0); CDMMIO16_ST(gmb,0xc3,0);
      *(uint16_t *)guest_at(0x801bde58) = 0;
      *(uint16_t *)guest_at(0x801bde5a) = 0;
    } else {
      if (s5 || (s3 & 2)) {
        CDMMIO16_ST(gmb,0xc2,*(uint16_t *)(blk + 8));
        *(uint16_t *)guest_at(0x801bde58) = *(uint16_t *)(blk + 8);
      }
      if (s5 || (s3 & 4)) {
        CDMMIO16_ST(gmb,0xc3,*(uint16_t *)(blk + 0xa));
        *(uint16_t *)guest_at(0x801bde5a) = *(uint16_t *)(blk + 0xa);
      }
    }
  }

  if (s4 || s6 || fp) cd_field_copy_gated(buf);
  if (f58) cd_mode_apply_deferred(CDREG32(0x801bde54));
  if (s4) cd_u16_table_store(0xd1,CDREG32(0x801bde4c),0);
  if (s7) {
    uint32_t gmb = CDREG32(0x801bdeb0);
    CDMMIO16_ST(gmb,0xd5,(uint16_t)(CDMMIO16(gmb,0xd5) | 0x80));
  }
  return 0;
}

/* cd_bitstream_decode — packed-bitstream decoder. a0 = guest ptr to stream
 * (0 = resume from state @0x801b6904), a1 = guest out ptr,
 * a2 = guest VLC table base. Faithful goto transliteration of the
 * guest CFG; delay-slot side effects (a1+=2, v0 shifts, stores)
 * preserved literally. Tables read via guest_at.
 * Disasm 0x8018ff10-0x80190258. */
void cd_bitstream_decode(int32_t a0,uint32_t a1,uint32_t a2)
{
  uint32_t t0,t1,t2,t3,t4,t5,t6,t7,t8,t9;
  uint32_t v0,v1,at,at2;
  uint32_t a0g,a1g,a2b,a3b;
  int v0neg;
#define FF_RD32(a)   (*(uint32_t *)guest_at(a))
#define FF_RD16(a)   (*(uint16_t *)guest_at(a))
#define FF_ST16(a,v) (*(uint16_t *)guest_at(a) = (uint16_t)(v))
#define FF_ST32(a,v) (*(uint32_t *)guest_at(a) = (uint32_t)(v))

  t1 = CDREG32(0x801b6900);
  a2b = a2 + 0x800;
  a3b = a2b + 0x10000;
  if (a0 == 0) {
    uint8_t *s = (uint8_t *)guest_at(0x801b6904);
    a0g = *(uint32_t *)(s + 0x00);
    a1g = *(uint32_t *)(s + 0x04);
    v0   = *(uint32_t *)(s + 0x08);
    v1   = *(uint32_t *)(s + 0x0c);
    t4   = *(uint32_t *)(s + 0x10);
    t5   = *(uint32_t *)(s + 0x14);
    t7   = *(uint32_t *)(s + 0x18);
    t8   = *(uint32_t *)(s + 0x1c);
    t9   = *(uint32_t *)(s + 0x20);
    t1 += t1;
    t6 = a1g + t1;
    goto L_f0;
  }
  t5 = 0; t7 = 0; t8 = 0; t9 = 0;
  t1 += t1;
  a0g = (uint32_t)a0;
  a1g = a1;
  t6 = a1g + t1;
  /* header parse */
  t1 = FF_RD32(a0g);
  t4 = (uint32_t)FF_RD16(a0g + 4) << 10;
  t2 = (uint32_t)FF_RD16(a0g + 6);
  v0 = FF_RD16(a0g + 8);
  v1 = FF_RD16(a0g + 0xa);
  t2 -= 3;
  if ((int32_t)t2 >= 0) t5 = 1;
  a0g += 0xc;
  v0 = (v0 << 16) | v1;
  v1 = 0;
  FF_ST32(a1g,t1);
  t1 = ((t1 & 0xffff) << 2) + 4 + a1g;
  FF_ST32(0x801b6928,t1);
  a1g += 2;

L_ffd4:
  t0 = v0 >> 0x16;                          /* delay of beqz t5 */
  if (t5 == 0) goto L_ac;
  /* ---- block d8: escape/sync symbol ---- */
  at = t0 ^ 0x3ff;
  a1g += 2;                                 /* ffe4 delay: always */
  if (at == 0) goto L_1f8;
  at = t5 - 3;
  at2 = a2b - 0x400;                        /* fff0 delay: always */
  if ((int32_t)at >= 0) at2 -= 0x400;
  t0 = ((v0 >> 0x18) << 2) + at2;
  t1 = FF_RD16(t0);
  t2 = FF_RD16(t0 + 2);
  t0 = 0;
  v0 <<= (t1 & 31);                         /* 9014 delay: always */
  if (t2 != 0) {
    at = 0x20 - t2;
    v0neg = ((int32_t)v0 < 0);              /* bltz pre-shift */
    t0 = v0 >> (at & 31);
    v0 <<= (t2 & 31);                       /* 9028 delay: always */
    if (!v0neg) {
      t3 = 0xffffffffu >> (at & 31);
      t0 -= t3;
    }
    v1 += t2;
  }
  v1 += t1;
  at = v1 & 0x10;
  v1 &= 0xf;
  if (at) {
    t1 = FF_RD16(a0g); a0g += 2;
    v0 |= t1 << (v1 & 31);
  }
  /* accumulator select: t5==1->t7, ==2->t8, >=3->t9 */
  at = t5 - 2;
  if ((int32_t)at > 0) {
    t1 = t9 + t0;
    t9 += t0;
  } else if (at == 0) {
    t1 = t8 + t0;
    t8 += t0;
  } else {
    t1 = t7 + t0;
    t7 += t0;
  }
  t1 = (t1 << 2) & 0x3ff;
  t1 |= t4;
  FF_ST16(a1g,t1);                          /* 90a0 delay: always */
  t5 += 1;
  if (t5 - 7 == 0) t5 -= 6;
  goto L_e4;

L_ac:
  /* ---- block ac: t5==0 escape variant ---- */
  at = t0 ^ 0x1ff;
  a1g += 2;                                 /* 90b4 delay: always */
  if (at == 0) goto L_1f8;
  v0 <<= 0xa;
  v1 += 0xa;
  at = v1 & 0x10;
  v1 &= 0xf;
  if (at) {
    t1 = FF_RD16(a0g); a0g += 2;
    v0 |= t1 << (v1 & 31);
  }
  t0 |= t4;
  FF_ST16(a1g,t0);
  goto L_e4;

L_e4:
  at = a1g - t6;
  a1g += 2;                                 /* delay: always */
  if ((int32_t)at >= 0) goto L_2028;
  goto L_f0;

L_f0:
  /* ---- main VLC symbol decode ---- */
  t0 = (v0 >> 0x13) << 3;
  t0 += a2b;
  t1 = FF_RD32(t0);
  at = t1 & 0xff;                           /* 9108 delay: always */
  if (t1 != 0) {
    t3 = FF_RD32(t0 + 4);
  } else {
    v0 <<= 8;
    v1 += 8;
    at2 = v1 & 0x10;
    v1 &= 0xf;
    if (at2) {
      t0 = FF_RD16(a0g); a0g += 2;
      v0 |= t0 << (v1 & 31);
    }
    t0 = ((v0 >> 0x17) << 2) + a3b;
    t1 = FF_RD32(t0);
    t3 = 0;
    at = t1 & 0xff;
  }
  v0 <<= (at & 31);
  v1 += at;
  at2 = v1 & 0x10;
  v1 &= 0xf;
  if (at2) {
    t0 = FF_RD16(a0g); a0g += 2;
    v0 |= t0 << (v1 & 31);
  }
  t1 >>= 0x10;
  if (t1 == 0x7c1f) goto L_d4;              /* 917c: skips a1+=2 */
  if (t1 == 0xfe00) { FF_ST16(a1g,t1); goto L_ffd4; }
  a1g += 2;                                 /* 9190 delay: always */
  if (t3 == 0) goto L_f0;
  t2 = t3 & 0xffff;
  if (t2 == 0x7c1f) goto L_d4;
  if (t2 == 0xfe00) { FF_ST16(a1g,t2); goto L_ffd4; }
  t2 = t3 >> 0x10;
  a1g += 2;                                 /* 91b4 delay: always */
  if (t2 == 0) goto L_f0;
  if (t2 == 0x7c1f) goto L_d4;
  if (t2 == 0xfe00) { FF_ST16(a1g,t2); goto L_ffd4; }
  a1g += 2;                                 /* 91d0 delay: always */
  goto L_f0;

L_d4:
  /* ---- 0x7c1f escape: emit v0>>16, refill 16 bits ---- */
  t0 = v0 >> 0x10;
  FF_ST16(a1g,t0);
  a1g += 2;
  t0 = FF_RD16(a0g); a0g += 2;
  v0 <<= 0x10;
  v0 |= t0 << (v1 & 31);
  goto L_f0;

L_1f8:
  /* ---- flush: pad 0xfe00 until a1g reaches limit ---- */
  t1 = CDREG32(0x801b6928);
  t0 = 0xfe00;
  for (;;) {
    at = a1g - t1;
    if ((int32_t)at >= 0) return;
    FF_ST16(a1g,t0);
    a1g += 2;
  }

L_2028:
  {
    uint8_t *s = (uint8_t *)guest_at(0x801b6904);
    *(uint32_t *)(s + 0x00) = a0g;
    *(uint32_t *)(s + 0x04) = a1g;
    *(uint32_t *)(s + 0x08) = v0;
    *(uint32_t *)(s + 0x0c) = v1;
    *(uint32_t *)(s + 0x10) = t4;
    *(uint32_t *)(s + 0x14) = t5;
    *(uint32_t *)(s + 0x18) = t7;
    *(uint32_t *)(s + 0x1c) = t8;
    *(uint32_t *)(s + 0x20) = t9;
  }
#undef FF_RD32
#undef FF_RD16
#undef FF_ST16
#undef FF_ST32
}

/* ======== batch32: CD register-window helpers (disasm-verified) ======== */

extern int      str_len(char *);
extern int32_t  cd_strcmp(const void *,const void *);
extern int32_t  cd_chan_cmd_pump(uint32_t,uint32_t,uint32_t);
extern void     cd_mmio_strobe_20(void);
extern void     cd_mmio_strobe_22(void);

/* cd_delay_3c — pure delay: 0x3c iterations of d *= 13 (d=13 seed). */
void cd_delay_3c(void)
{
  volatile uint32_t d = 13;
  int i;
  for (i = 0; i < 0x3c; i++) d = d * 13;
  (void)d;
}

/* cd_reg_stream_push — streamed u16 register push: copies a1 bytes worth of
 * u16s from guest src a0 into reg block +0x1a8 (0x40-int8_t chunks),
 * pulses +0x1aa bits 0x10, waits +0x1ae bit 0x400 clear, then waits
 * (+0x1ae & 0x7ff) == saved status. b[] = u16 view of block @0x801bdeb0.
 * Disasm 0x8019b620-0x8019b7dc. */
void cd_reg_stream_push(uint32_t a0,uint32_t a1)
{
  /* Clean port of 0x8019b620-0x8019b7dc (re-verified against generated).
   * b = *cell 0x801bdeb0 (reg block); dec8 = u16 @0x801bdec8 is staged
   * into b+0x1a6 before the first cd_delay_3c. Outer loop: chunk =
   * min(cnt,64) u16s copied src->b+0x1a8, +0x1aa pulsed ((h&~0x30)|0x10),
   * delay, bounded poll for +0x1ae bit 0x400 clear (0xf01 cap ->
   * printf 0x8018e550 fmt 0x80125be8 arg 0x80125c08), cnt -= chunk,
   * two more delays. Tail: +0x1aa &= ~0x30, bounded poll for
   * (+0x1ae & 0x7ff) == saved status (cap -> printf arg 0x80125c1c).
   * Polls use br2_irq_poll so the CD device model advances guest cycles;
   * printf goes through a real guest frame + call_guest4. */
  uint32_t b    = br2_ld32(0x801bdeb0);
  uint32_t dec8 = (uint32_t)br2_ld16(0x801bdec8);
  uint32_t cnt  = a1;                          /* s1: int8_t count */
  uint32_t src  = a0;                          /* s2: guest src ptr */
  uint32_t want = (uint32_t)br2_ld16(b + 0x1ae) & 0x7ffu;  /* s3 */
  uint32_t chunk, i, v1, h;

  br2_st16(b + 0x1a6, (uint16_t)dec8);
  cd_delay_3c();

  while (cnt != 0) {
    chunk = (cnt < 65u) ? cnt : 64u;
    i = 0;
    if ((int32_t)chunk > 0) {
      do {
        uint32_t w = (uint32_t)br2_ld16(src);
        src += 2;
        i += 2;
        br2_st16(b + 0x1a8, (uint16_t)w);
      } while (i < chunk);
    }
    h = (uint32_t)br2_ld16(b + 0x1aa);
    br2_st16(b + 0x1aa, (uint16_t)((h & 0xffcfu) | 0x10u));
    cd_delay_3c();
    if (((uint32_t)br2_ld16(b + 0x1ae) & 0x400u) != 0) {
      v1 = 0;
      for (;;) {
        v1++;
        if (v1 >= 3841u) {
          uint32_t fr = br2_guest_frame(0x28);
          br2_st32(fr + 0x20, clean_cpu_state()->gpr[31]);
          (void)call_guest4(0x8018e550, 0x80125be8u, 0x80125c08u, 0, 0);
          if (br2_bail_pending()) { br2_guest_frame_abandon(); return; }
          br2_guest_frame_free();
          break;
        }
        b = br2_ld32(0x801bdeb0);
        if (((uint32_t)br2_ld16(b + 0x1ae) & 0x400u) == 0) break;
        if (br2_bail_pending()) return;
        br2_irq_poll();
      }
    }
    cnt -= chunk;
    cd_delay_3c();
    cd_delay_3c();
    b = br2_ld32(0x801bdeb0);
  }

  h = (uint32_t)br2_ld16(b + 0x1aa);
  br2_st16(b + 0x1aa, (uint16_t)(h & 0xffcfu));
  if (((uint32_t)br2_ld16(b + 0x1ae) & 0x7ffu) != (want & 0xffffu)) {
    v1 = 0;
    for (;;) {
      v1++;
      if (v1 >= 3841u) {
        uint32_t fr = br2_guest_frame(0x28);
        br2_st32(fr + 0x20, clean_cpu_state()->gpr[31]);
        (void)call_guest4(0x8018e550, 0x80125be8u, 0x80125c1cu, 0, 0);
        if (br2_bail_pending()) { br2_guest_frame_abandon(); return; }
        br2_guest_frame_free();
        break;
      }
      b = br2_ld32(0x801bdeb0);
      if (((uint32_t)br2_ld16(b + 0x1ae) & 0x7ffu) == (want & 0xffffu)) break;
      if (br2_bail_pending()) return;
      br2_irq_poll();
    }
  }
}

/* cd_reg_window_op — register-window op dispatch (ops 0..3), returns 0 or
 * -2 on poll timeout. Faithful port of generated 0x8019B944 (16B thunk
 * spilling a0-a3 to the caller arg-save area) + 0x8019B954 (624B / 32
 * blocks, decoded insn-by-insn against generated/SLES_017.22_full_47.c).
 *
 * Frame: callee sp-=0x18, s0@16/ra@20/a0@24; s0=sp+28 points at the
 * thunk-saved a1 (param a1), +4 more reaches saved a2 — reproduced as
 * direct param use since nothing else consumes the spill slots.
 *
 * Dispatch (a2 is preset to 1 and doubles as the op-1 compare, the
 * op-0 DF00 store value, and the op-3 "==1" test):
 *   a0==0 -> op0  a0==1 -> op1  a0==2 -> op2  a0==3 -> op3  else ret 0.
 *
 * op1 (B9D0): sw 0 -> 0x801BDF00; poll lhu(blk+0x1a6)==lhu(0x801BDEC8)
 *   (blk = *(u32*)0x801BDEB0, cached in-reg across the loop, cap 3841
 *   -> -2); tail: v = lhu(blk2+0x1aa); sh (v & 0xFFCF) | 0x20.
 * op0 (BA3C): identical except sw 1 -> 0x801BDF00 and tail sh v | 0x30.
 * op2 (B9A4): v = a1_param >> (*(u32*)0x801BDED8 & 31) [srlv];
 *   sh v -> 0x801BDEC8 and -> blk+0x1a6.
 * op3 (BAA4): sel = (*(u32*)0x801BDF00 != 1) ? 32 : 48; poll
 *   (lhu(blk+0x1aa) & 0x30) == sel (cap 3841 -> -2); then jal
 *   cd_mmio_strobe_22 (DF00==1, ra=0x8019BB18) or cd_mmio_strobe_20
 *   (!=1, ra=0x8019BB28); tail BB2C programs the sector-read cells:
 *   *(u32*)0x801BDF04 = a1_param; v = (a2_param>>6) + ((a2_param&0x3f)!=0);
 *   *(u32*)0x801BDF08 = v; *(u32*)*(u32*)0x801BDEB4 = a1_param;
 *   *(u32*)*(u32*)0x801BDEB8 = (v<<16)|0x10; cmd = (DF00==1) ?
 *   0x01000200 : 0x01000201; *(u32*)*(u32*)0x801BDEBC = cmd; ret 0.
 *
 * The three 3841-iteration waits are device-deadline polls on the SPU
 * register window (blk resolves to the 0x1F801C00-family MMIO) — the
 * proven br2_irq_poll quantum advances guest cycles so the CD deferred
 * coroutine can move the status bits, exactly like the generated
 * per-block psx_check_interrupts. blk-relative u16s go through
 * CDMMIO16* (never guest_at — the block base is a hardware address);
 * the 0x801BDExx cells are plain guest RAM via the br2_ldN/br2_stN
 * accessors. */
uint32_t cd_reg_window_op(uint32_t a0,uint32_t a1,uint32_t a2)
{
  uint32_t fr = br2_guest_frame(0x18);
  uint32_t blk, v, i, tgt, sel, cmd;
  (void)fr;

  if (a0 == 0u || a0 == 1u) {
    /* op0/op1 — blocks B9D0/BA3C entry + B9F4..BA14/BA60..BA80 poll/tail.
     * Guest order: blk=*(DEB0); tgt=lhu(DEC8); v=lhu(blk+0x1a6);
     * sw (a0?1:0) -> DF00; if v!=tgt poll {i++; i>=3841 -> -2;
     * irq_poll; v=lhu(blk+0x1a6)} while v!=tgt; then tail on blk2. */
    blk = br2_ld32(0x801BDEB0u);
    tgt = br2_ld16(0x801BDEC8u);              /* lhu — cell is RAM */
    v = CDMMIO16(blk,0xd3);                   /* blk+0x1a6 */
    br2_st32(0x801BDF00u, a0 == 0u ? 1u : 0u);/* sw a2reg(=1)/zero */
    if (v != tgt) {
      i = 0;
      for (;;) {
        i++;
        if (i >= 3841u) { br2_guest_frame_free(); return 0xFFFFFFFEu; }
        if (br2_bail_pending()) { br2_guest_frame_abandon(); return 0; }
        br2_irq_poll();
        v = CDMMIO16(blk,0xd3);
        if (v == tgt) break;
      }
    }
    blk = br2_ld32(0x801BDEB0u);              /* tail reloads the cell */
    v = CDMMIO16(blk,0xd5);                   /* blk+0x1aa */
    if (a0 == 1u) v = (v & 0xFFCFu) | 0x20u;  /* op1: (v&~0x30)|0x20 */
    else         v = v | 0x30u;              /* op0: v|0x30 */
    CDMMIO16_ST(blk,0xd5,(uint16_t)v);
    br2_guest_frame_free();
    return 0;
  }

  if (a0 == 2u) {
    /* op2 — block B9A4: v = a1 >> (*(u32*)0x801BDED8 & 31); sh to
     * 0x801BDEC8 and blk+0x1a6. */
    uint32_t sh = br2_ld32(0x801BDED8u);
    uint32_t w = a1 >> (sh & 31u);            /* srlv — logical */
    blk = br2_ld32(0x801BDEB0u);
    br2_st16(0x801BDEC8u, (uint16_t)w);
    CDMMIO16_ST(blk,0xd3,(uint16_t)w);
    br2_guest_frame_free();
    return 0;
  }

  if (a0 == 3u) {
    /* op3 — blocks BAA4/BABC/BAFC + strobe jal + BB2C tail. */
    sel = (br2_ld32(0x801BDF00u) != 1u) ? 32u : 48u;
    blk = br2_ld32(0x801BDEB0u);
    v = CDMMIO16(blk,0xd5) & 0x30u;
    if (v != sel) {
      i = 0;
      for (;;) {
        i++;
        if (i >= 3841u) { br2_guest_frame_free(); return 0xFFFFFFFEu; }
        if (br2_bail_pending()) { br2_guest_frame_abandon(); return 0; }
        br2_irq_poll();
        v = CDMMIO16(blk,0xd5) & 0x30u;
        if (v == sel) break;
      }
    }
    /* jal cd_mmio_strobe_22 (DF00==1, ra=0x8019BB18) /
     *     cd_mmio_strobe_20 (!=1, ra=0x8019BB28); a2 := 0x01000000. */
    if (br2_ld32(0x801BDF00u) == 1u)
      (void)cd_jal(0x8019BE50u, 0x8019BB18u);
    else
      (void)cd_jal(0x8019BE28u, 0x8019BB28u);
    if (br2_bail_pending()) { br2_guest_frame_abandon(); return 0; }
    /* BB2C tail — sector-read cell programming. */
    br2_st32(0x801BDF04u, a1);
    {
      uint32_t q = a2 >> 6;
      uint32_t r = a2 & 0x3Fu;
      uint32_t w = q + (r != 0u ? 1u : 0u);
      uint32_t p;
      br2_st32(0x801BDF08u, w);
      p = br2_ld32(0x801BDEB4u);
      br2_st32(p, a1);                          /* sw a1 -> *DEB4 ptr */
      v = br2_ld32(0x801BDF08u);
      p = br2_ld32(0x801BDEB8u);
      br2_st32(p, (v << 16) | 0x10u);
      cmd = (br2_ld32(0x801BDF00u) == 1u) ? 0x01000200u : 0x01000201u;
      p = br2_ld32(0x801BDEBCu);
      br2_st32(p, cmd);
    }
    br2_guest_frame_free();
    return 0;
  }

  /* invalid op — v0=0 epilogue (blocks B984/B99C). */
  br2_guest_frame_free();
  return 0;
}

/* cd_field_copy_gated — gated u16 field copy: fl = *(u32*)a0; fl==0 applies
 * all 31 masked fields plus the sign-bit field; else each bit enables
 * one u16 copy p[src] -> regblock[dst]. dst indices 0xe0..0xff of the
 * u16 block @*(u32*)0x801bdeb0. Disasm 0x8019c5d0-0x8019ca9c. */
void cd_field_copy_gated(void *a0)
{
  static const struct { uint32_t mask; uint32_t src; uint32_t dst; } T[] = {
    {0x00000001,0x04,0x1c0},{0x00000002,0x06,0x1c2},
    {0x00000004,0x08,0x1c4},{0x00000008,0x0a,0x1c6},
    {0x00000010,0x0c,0x1c8},{0x00000020,0x0e,0x1ca},
    {0x00000040,0x10,0x1cc},{0x00000080,0x12,0x1ce},
    {0x00000100,0x14,0x1d0},{0x00000200,0x16,0x1d2},
    {0x00000400,0x18,0x1d4},{0x00000800,0x1a,0x1d6},
    {0x00001000,0x1c,0x1d8},{0x00002000,0x1e,0x1da},
    {0x00004000,0x20,0x1dc},{0x00008000,0x22,0x1de},
    {0x00010000,0x24,0x1e0},{0x00020000,0x26,0x1e2},
    {0x00040000,0x28,0x1e4},{0x00080000,0x2a,0x1e6},
    {0x00100000,0x2c,0x1e8},{0x00200000,0x2e,0x1ea},
    {0x00400000,0x30,0x1ec},{0x00800000,0x32,0x1ee},
    {0x01000000,0x34,0x1f0},{0x02000000,0x36,0x1f2},
    {0x04000000,0x38,0x1f4},{0x08000000,0x3a,0x1f6},
    {0x10000000,0x3c,0x1f8},{0x20000000,0x3e,0x1fa},
    {0x40000000,0x40,0x1fc} };
  uint8_t *p = (uint8_t *)a0;
  uint32_t blk;
  uint32_t fl = *(uint32_t *)p;
  int all = (fl < 1u);
  uint32_t i;
  blk = CDREG32(0x801bdeb0);
  for (i = 0; i < sizeof(T)/sizeof(T[0]); i++) {
    if (all || (fl & T[i].mask))
      CDMMIO16_ST(blk,T[i].dst >> 1,*(uint16_t *)(p + T[i].src));
  }
  /* last field gated by bgez (signed fl >= 0) or all */
  if (all || ((int32_t)fl >= 0))
    CDMMIO16_ST(blk,0xff,*(uint16_t *)(p + 0x42));      /* +0x1fe */
}

/* cd_mode_apply_deferred — mode apply with deferred-pause save/restore.
 * Faithful port of generated 0x8019CE10 (404B / 24 blocks, decoded
 * insn-by-insn against generated/SLES_017.22_full_47.c:16787).
 *
 * Frame 0x38 (s0-s5 + ra + a zero slot @16(sp) = saved DEE8, reproduced
 * as a C local — nothing external reads the spill). Prologue: mode a0
 * must be <10u (sltiu) else ret -1; ent = *(u32*)(0x801BE440 + mode*4);
 * jal cd_range_table_member(ent) — nonzero -> ret -1.
 *
 * s1/s2 init (sh = *(u32*)0x801BDED8, sllv &31):
 *   mode==0: s1 = 0x10<<sh,        s2 = 0xFFF0<<sh
 *   mode!=0: v0 = 0x10000 - ent    (CE70 delay slot lui v0,1 then subu
 *           v0,v0,a0 — NOT -ent),  s1 = (0x10000-ent)<<sh, s2 = ent<<sh
 *           (ent reloaded like guest)
 * Deferred-pause save: s4 = *(0x801BDECC); if ==1 { s5=1; DECC=0 }.
 * s3=1 always; pend = *(0x801BDEE8); if !=0 { saved=pend; DEE8=0 }.
 *
 * Chunk loop (back-edge CF48->CEFC while s3!=0):
 *   s0c = (s1 < 1025u) ? s1 : 0x400   [sltiu; s3=0 only on last chunk]
 *   jal cd_reg_window_op(2, s2, ?)     ra=0x8019CF18 — a1=s2, a2 leftover
 *   jal cd_reg_window_op(1, ?, ?)      ra=0x8019CF20 — only a0 set;
 *                                      a1/a2 are callee leftovers exactly
 *                                      like the guest (op2 writes neither)
 *   jal cd_reg_window_op(3, 0x801BDF20, s0c)  ra=0x8019CF34
 *   a0 = *(0x801BDE38); s1 -= 0x400; s2 += 0x400 (delay slot of next jal)
 *   jal 0x8019CFB0 (BIOS B0/0xa tramp) ra=0x8019CF48
 * Epilogue: if (s5) DECC = s4; if (saved) DEE8 = saved; ret 0.
 * All jals go through cd_jal so a0-a3 keep callee leftovers like real
 * MIPS jal; the B0/0xa tramp is a BIOS-adjacent jal TARGET (allowed). */
int32_t cd_mode_apply_deferred(uint32_t a0)
{
  uint32_t fr = br2_guest_frame(0x38);
  uint32_t mode = a0;
  uint32_t ent, sh, s1, s2, s0c, s3, s4, s5, saved;
  CPUState *cpu = clean_cpu_state();
  (void)fr;
  CDFN(CDFN_MODE_APPLY_DEFERRED,a0);

  if (mode >= 10u) { br2_guest_frame_free(); return -1; }
  ent = br2_ld32(0x801BE440u + (mode << 2));
  cpu->gpr[4] = ent;
  (void)cd_jal(0x8019C060u, 0x8019CE60u);        /* jal cd_range_table_member */
  if (br2_bail_pending()) { br2_guest_frame_abandon(); return 0; }
  if (cpu->gpr[2] != 0) { br2_guest_frame_free(); return -1; }

  sh = br2_ld32(0x801BDED8u);
  if (mode == 0u) {
    s1 = 16u << (sh & 31u);
    s2 = 0xFFF0u << (sh & 31u);
  } else {
    ent = br2_ld32(0x801BE440u + (mode << 2));  /* guest reloads table[mode] */
    s1 = (0x10000u - ent) << (sh & 31u);        /* (0x10000-ent)<<sh */
    s2 = ent << (sh & 31u);
  }

  s4 = br2_ld32(0x801BDECCu);
  s5 = 0;
  if (s4 == 1u) { s5 = 1; br2_st32(0x801BDECCu, 0); }
  s3 = 1;
  saved = 0;
  {
    uint32_t pend = br2_ld32(0x801BDEE8u);
    if (pend != 0) { saved = pend; br2_st32(0x801BDEE8u, 0); }
  }

  for (;;) {
    /* CEFC/CF04: chunk size + last-chunk flag (s3=0 when s1<1025). */
    if (s1 < 1025u) { s0c = s1; s3 = 0; } else { s0c = 1024u; }
    /* CF0C: op2 with a1=s2 (a2 = live leftover, untouched like guest). */
    cpu->gpr[4] = 2; cpu->gpr[5] = s2;
    (void)cd_jal(0x8019B944u, 0x8019CF18u);
    if (br2_bail_pending()) { br2_guest_frame_abandon(); return 0; }
    /* CF18: op1 — only a0 is set; a1/a2 keep call1 leftovers (guest op2
     * leaves both untouched, and so does the clean port). */
    cpu->gpr[4] = 1;
    (void)cd_jal(0x8019B944u, 0x8019CF20u);
    if (br2_bail_pending()) { br2_guest_frame_abandon(); return 0; }
    /* CF20: op3 — a1=0x801BDF20, a2=s0c (chunk), a0=3. */
    cpu->gpr[4] = 3; cpu->gpr[5] = 0x801BDF20u; cpu->gpr[6] = s0c;
    (void)cd_jal(0x8019B944u, 0x8019CF34u);
    if (br2_bail_pending()) { br2_guest_frame_abandon(); return 0; }
    /* CF34: a0=*(0x801BDE38); s1-=0x400; s2+=0x400 (jal delay slot). */
    cpu->gpr[4] = br2_ld32(0x801BDE38u);
    s1 -= 0x400u;
    s2 += 0x400u;
    (void)cd_jal(0x8019CFB0u, 0x8019CF48u);        /* jal B0/0xa BIOS tramp */
    if (br2_bail_pending()) { br2_guest_frame_abandon(); return 0; }
    if (s3 != 0) continue;                        /* CF48: loop while s3 */
    break;
  }
  /* CF50/CF58: restore DECC when it was saved; CF60/CF70: restore DEE8. */
  if (s5 != 0) br2_st32(0x801BDECCu, s4);
  if (saved != 0) br2_st32(0x801BDEE8u, saved);
  br2_guest_frame_free();
  return 0;
}
/* cd_curve_lookup — 4-nibble curve lookup: combines a3+a1, derives a
 * signed mod-3 index into u16 table @0x801be320 and a 7-bit index into
 * @0x801be338, multiplies, then conditional round-up shift.
 * Faithful mult hi/lo + sra/srlv/sllv(&31) port.
 * Disasm 0x8019e0d0-0x8019e19c. */
uint32_t cd_curve_lookup(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3)
{
  int32_t sum, q, rem, idx, v0, a1r, sh;
  uint32_t lu;
  int64_t p;

  sum = (int32_t)(a3 + a1) & 0xffff;      /* v1 = (a3+a1)&0xffff */
  a2 = a2 + (uint32_t)(sum >> 7);         /* srl logical */
  a2 = a2 - a0;
  a2 <<= 16;
  a0 = (int32_t)a2 >> 16;                 /* sra: (int16) */
  /* a1 = ((a0 * 0x2aaaaaab) >> 32 >> 1) - (a2 >> 31)  => a0/3-ish */
  p = (int64_t)a0 * (int64_t)(int32_t)0x2aaaaaab;
  q = (int32_t)((int32_t)(p >> 32) >> 1) - ((int32_t)a2 >> 31); /* sra sign */
  rem = a0 - q * 3;
  idx = q - 2;
  if (rem < 0) {                          /* bgez on rem<<16 */
    idx = q - 3;
    lu = (uint32_t)(rem + 0xc);
  } else {
    lu = (uint32_t)rem;
  }
  /* w = (int16)lu * 2  ( (lu<<16) sra 15 ) */
  {
    int32_t w = (int32_t)((int32_t)(lu << 16) >> 15);
    uint32_t i1 = (uint32_t)(0x801c0000 + (uint32_t)w - 0x1ce0);
    uint32_t i2 = (uint32_t)(0x801c0000 + ((sum & 0x7f) << 1) - 0x1cc8);
    int32_t prod;
    /* mult $v1,$v0: both lhu (zero-extended u16), signed lo product */
    prod = (int32_t)CDREG16(i1) * (int32_t)CDREG16(i2);
    a1r = prod >> 16;                     /* sra */
    v0 = (int32_t)(int16_t)idx;           /* idx<<16>>16 */
    if (v0 < 0) {
      int32_t n = -v0;
      sh = n - 1;
      a1r += 1 << (sh & 31);              /* sllv */
      a1r = (int32_t)((uint32_t)a1r >> (n & 31)); /* srlv logical */
    } else {
      a1r = 0x3fff;
    }
  }
  return (uint32_t)a1r & 0xffff;
}

/* cd_xchg_fec24 — get-and-set u32 @0x801fec24, returns previous. */
uint32_t cd_xchg_fec24(uint32_t a0)
{
  uint32_t *p = (uint32_t *)guest_at(0x801fec24);
  uint32_t old = *p;
  *p = a0;
  return old;
}

/* cd_status_map — status-code map: 0->0, 1->2, 2->1, 4->3,
 * anything else -> a0|0x8000. Disasm 0x801a00bc-0x801a010c. */
uint32_t cd_status_map(uint32_t a0)
{
  if (a0 == 1) return 2;
  if ((int32_t)a0 < 2) {
    if (a0 == 0) return 0;
    return a0 | 0x8000;
  }
  if (a0 == 2) return 1;
  if (a0 == 4) return 3;
  return a0 | 0x8000;
}

/* cd_file_header_copy — 6-int8_t header copy from payload @0x80125f6c into
 * host buf a1, then bytes 2/3 = two nibble+'0' chars of a0
 * (a0<0 -> +0xf before >>4; lo nibble = a0 - (q<<4)). Disasm verified. */
void cd_file_header_copy(uint32_t a0,void *a1)
{
  uint8_t *d = (uint8_t *)a1;
  int32_t v = (int32_t)a0;
  int32_t q = v;
  memcpy(d,guest_at(0x80125f6c),6);
  if (q < 0) q += 0xf;
  q >>= 4;                               /* sra */
  d[2] = (uint8_t)(q + 0x30);
  d[3] = (uint8_t)(v - (q << 4) + 0x30);
}

/* FUN_801a0260 / FUN_801a05f0 — SKIPPED: pure BIOS-call trampolines
 * (jr $t2=0xb0, t1=0x43 / t2=0xa0,t1=0xac then b0 entries). No body. */

/* cd_file_record_lookup — file-record lookup: copies tag bytes (<0x3b signed)
 * from a0 into buf @0x801fec48, scans 0x50-stride table
 * @*(u32*)0x150..+count*80 (@0x154 count) comparing names via
 * cd_strcmp; phase1 hit -> *0x801fec40 = entry+0x34; phase2
 * strcmp==0 -> entry+0x34 = 0x801a041c; tail calls BIOS svc
 * B0/0x42 trampoline (FUN_801a0520) with (a0,a1) unless phase1 miss
 * (returns 0). a0/a1 are HOST pointers (callers pass stack bufs).
 * Reads kernel cells 0x150/0x154 via guest_at.
 * Disasm 0x801a0280-0x801a0418. */
uint32_t cd_file_record_lookup(void *a0,void *a1)
{
  /* Faithful port of 0x801a0280-0x801a0418. Pure guest logic (bounded
   * int8_t-copy + two 0x50-stride table scans through clean cd_strcmp)
   * plus ONE BIOS kernel call: jal 0x801a0520 = `li t2,0xb0; jr t2;
   * li t1,0x42` trampoline -> B0/0x42 with live a0-a3, reproduced via
   * bios_svc6 exactly like the other inline trampolines. No crit
   * section, no kernel-object setup, no polling — the unsafe CPS-frame
   * pattern does not apply. */
  uint32_t s2 = (uint32_t)guest_of(a0);   /* tag string guest addr */
  uint32_t s3 = (uint32_t)guest_of(a1);
  uint8_t *d = (uint8_t *)guest_at(0x8020ec48);
  uint32_t base, sz, n, end, cur;
  uint32_t a2live;
  int found = 0;

  /* tag copy: enter loop only when first signed int8_t >= 0x3b; copy while
   * the NEXT signed int8_t is >= 0x3b (slti/beqz polarity verified vs
   * 0x801a02ac-0x801a02d4), NUL-terminate. */
  if ((int8_t)br2_ld8(s2) < 0x3b) {
    *d = 0;
  } else {
    uint32_t v1 = s2;
    for (;;) {
      *d++ = br2_ld8(v1); v1++;
      if ((int8_t)br2_ld8(v1) < 0x3b) break;
    }
    *d = 0;
  }

  /* end = base + ((size*0xCCCCCCCD)>>32 >>6) * 80  (multu/mfhi/srl/sll) */
  sz   = br2_ld32(0x154);
  base = br2_ld32(0x150);
  n    = (uint32_t)(((uint64_t)sz * 0xCCCCCCCDull) >> 32) >> 6;
  end  = base + n * 80;
  a2live = (uint32_t)(((uint64_t)sz * 0xCCCCCCCDull) >> 32); /* mfhi $a2 */

  /* phase 1: first strcmp hit stores entry+0x34 -> 0x8020ec40 */
  for (cur = base; cur < end; cur += 0x50) {
    uint32_t name = br2_ld32(cur);
    if (name != 0 &&
        cd_strcmp((const void *)guest_at(name), (const void *)d) == 0) {
      br2_st32(0x8020ec40, br2_ld32(cur + 0x34));
      found = 1;
      break;
    }
  }
  if (!found) return 0;             /* v0=0 -> epilogue */

  /* phase 2: rescan; a strcmp hit writes 0x801a041c -> entry+0x34.
   * Either way falls into the BIOS call. */
  for (cur = base; cur < end; cur += 0x50) {
    uint32_t name = br2_ld32(cur);
    if (name != 0 &&
        cd_strcmp((const void *)guest_at(name), (const void *)d) == 0) {
      br2_st32(cur + 0x34, 0x801a041c);
      break;
    }
  }
  {
    /* live a3 = caller's $a3 (function never writes it) */
      CPUState *cpu = clean_cpu_state();
    uint32_t a3 = cpu ? cpu->gpr[7] : 0;
    return bios_svc6(0xb0,0x42,s2,s3,a2live,a3);
  }
}
/* cd_arg_ptr — resolve a string/buffer argument that may arrive as a
 * host pointer into guest RAM, a guest u32 address, or a raw host
 * pointer. guest_of() detects guest-backed host pointers; the KSEG0 /
 * scratchpad range check catches bare guest addresses. */
static void *cd_arg_ptr(uintptr_t v)
{
  uint32_t g = guest_of((const void *)v);
  if (g != 0) return guest_at(g);
  if (v >= 0x80000000u && v < 0x80200000u) return guest_at((uint32_t)v);
  if ((v & 0xfffff000u) == 0x1f800000u) return guest_at((uint32_t)v);
  return (void *)v;
}

/* cd_strcat_guard — strcat with overlap guard: returns 0 when either arg
 * NULL or end(dst)==end(src); else appends src at dst's NUL.
 * a0 is a HOST pointer; a1 may be a host pointer, guest-backed pointer,
 * or bare guest u32 (resolved via cd_arg_ptr).
 * Disasm 0x801a0530-0x801a05d0. */
void cd_strcat_guard(void *a0,uintptr_t a1)
{
  char *s1 = (char *)a0;
  char *s2 = (char *)cd_arg_ptr(a1);
  char *v1, *send, *send2;
  int c;
  if (s1 == NULL || s2 == NULL) return;
  c = str_len(s1);                 /* strlen; v0 in delay-add */
  send = s1 + c;                        /* s0 = end of s1 */
  c = str_len(s2);
  send2 = s2 + c;
  if (send == send2) return;
  v1 = s1;
  c = (int8_t)*v1;                      /* lb */
  s1 = v1 + 1;                          /* delay slot: always */
  if (c != 0) {
    for (;;) {
      c = (int8_t)*s1;
      s1 = s1 + 1;                      /* delay slot: always */
      if (c == 0) break;
    }
  }
  s1 = s1 - 1;
  for (;;) {
    uint8_t d = (uint8_t)*s2;
    s2++;
    *s1 = (char)d;
    s1++;
    if (d == 0) break;
  }
}

/* FUN_801a05f0 — SKIPPED: BIOS trampoline cluster head
 * (jr $t2=0xa0 t1=0xac / jr $t2=0xb0 t1=0x4e / t1=0x50). */

/* cd_chan_cmd_burst — channel command pump: 15 rounds building a 0x20-int8_t
 * request {0xa0,0,0xffff,0...} at 0x801fec70+i*0x20, copying it to the
 * 0x80-int8_t work buf @0x801feea0 and calling cd_chan_cmd_pump(a0,i+1,buf);
 * all must return 1. Then 20 rounds storing -1 at 0x801fee50+i*4,
 * work buf = that u32, cd_chan_cmd_pump(a0,i+0x10,buf); final sends
 * {'M','C'} header, returns (ret==1). Disasm 0x801a07a4-0x801a09ac. */
uint32_t cd_chan_cmd_burst(uint32_t a0)
{
  uint32_t s4 = a0;
  int s2;
  for (s2 = 0; s2 < 0xf; s2++) {
    uint32_t blk = 0x801fec70 + (uint32_t)s2 * 0x20;
    cd_bzero(0x801feea0,0x80);
    cd_bzero(blk,0x20);
    CDREG32(blk)     = 0xa0;
    CDREG32(blk + 4) = 0;
    CDREG16(blk + 8) = 0xffff;
    memcpy(guest_at(0x801feea0),guest_at(blk),0x20);
    if (cd_chan_cmd_pump(s4,(uint32_t)(s2 + 1),0x801feea0) != 1) return 0;
  }
  for (s2 = 0; s2 < 0x14; s2++) {
    uint32_t cell = 0x801fee50 + (uint32_t)s2 * 4;
    CDREG32(cell) = 0xffffffff;
    cd_bzero(0x801feea0,0x80);
    CDREG32(0x801feea0) = CDREG32(cell);
    if (cd_chan_cmd_pump(s4,(uint32_t)(s2 + 0x10),0x801feea0) != 1) return 0;
  }
  cd_bzero(0x801feea0,0x80);
  *(uint8_t *)guest_at(0x801feea0)     = 0x4d;   /* 'M' */
  *(uint8_t *)guest_at(0x801feea0 + 1) = 0x43;   /* 'C' */
  return cd_chan_cmd_pump(s4,0,0x801feea0) == 1 ? 1u : 0u;
}

/* cd_job_slot_alloc — job-slot allocator: idx = *@0x801be768 + 1; >=4 ->
 * printf 0x80125f98 + return; else store idx, fn ptr a0 ->
 * @0x801fef60+idx*4, zero 16-int8_t entry @0x801fef20+idx*0x10.
 * a1 unused (overwritten in guest). Disasm 0x801a0a10-0x801a0a88. */
void cd_job_slot_alloc(uint32_t a0,uint32_t a1)
{
  uint32_t idx, i;
  (void)a1;
  idx = CDREG32(0x801be768);
  idx = idx + 1;
  if ((int32_t)idx < 4) {
    CDREG32(0x801be768) = idx;
    CDREG32(0x801fef60 + idx * 4) = a0;
    {
      uint32_t *e = (uint32_t *)guest_at(0x801fef20 + idx * 0x10);
      for (i = 0; i < 4; i++) e[i] = 0;
    }
  } else {
    printf_forward((const char *)guest_at(0x80125f98));
  }
}

/* cd_pending_args_pump — pump 8 pending channel args through cd_seek_state_machine
 * (cells 0x801fef70..8c), then FIFO-clear both 4-deep queues
 * 0x801fef90-9c and 0x801fefa0-ac (each = shift-down by one slot,
 * head zeroed). Disasm 0x801a0e90-0x801a0f94. */
void cd_pending_args_pump(void)
{
  int i;
  for (i = 0; i < 8; i++)
    cd_seek_state_machine(CDREG32(0x801fef70 + (uint32_t)i * 4));
  CDREG32(0x801fef9c) = 0;
  CDREG32(0x801fef98) = CDREG32(0x801fef9c);
  CDREG32(0x801fef94) = CDREG32(0x801fef98);
  CDREG32(0x801fef90) = CDREG32(0x801fef94);
  CDREG32(0x801fefac) = 0;
  CDREG32(0x801fefa8) = CDREG32(0x801fefac);
  CDREG32(0x801fefa4) = CDREG32(0x801fefa8);
  CDREG32(0x801fefa0) = CDREG32(0x801fefa4);
}

/* cd_queue_a_status — queue-A status: s0 = q90 + q94*2 + q98*4 + q9c*8
 * (busy-poll until nonzero), drains 4 args via cd_seek_state_machine
 * (0x801fef80..8c), FIFO-clears queue A, returns s0>>1 (sra).
 * Disasm 0x801a0f98-0x801a106c. */
uint32_t cd_queue_a_status(void)
{
  uint32_t s0;
  for (;;) {
    if (br2_bail_pending()) return 0;
    s0 = CDREG32(0x801fef90)
       + (CDREG32(0x801fef94) << 1)
       + (CDREG32(0x801fef98) << 2)
       + (CDREG32(0x801fef9c) << 3);
    if (s0 != 0) break;
  }
  cd_seek_state_machine(CDREG32(0x801fef80));
  cd_seek_state_machine(CDREG32(0x801fef84));
  cd_seek_state_machine(CDREG32(0x801fef88));
  cd_seek_state_machine(CDREG32(0x801fef8c));
  CDREG32(0x801fef9c) = 0;
  CDREG32(0x801fef98) = CDREG32(0x801fef9c);
  CDREG32(0x801fef94) = CDREG32(0x801fef98);
  CDREG32(0x801fef90) = CDREG32(0x801fef94);
  return (uint32_t)((int32_t)s0 >> 1);
}

/* cd_queue_b_status — queue-B status: s0 = qa0 + qa4*2 + qa8*4 + qac*8
 * (busy-poll until nonzero), drains 4 args via cd_seek_state_machine
 * (0x801fef70..7c), FIFO-clears queue B, returns s0>>1 (sra).
 * Disasm 0x801a1070-0x801a1144. */
uint32_t cd_queue_b_status(void)
{
  uint32_t s0;
  for (;;) {
    if (br2_bail_pending()) return 0;
    s0 = CDREG32(0x801fefa0)
       + (CDREG32(0x801fefa4) << 1)
       + (CDREG32(0x801fefa8) << 2)
       + (CDREG32(0x801fefac) << 3);
    if (s0 != 0) break;
  }
  cd_seek_state_machine(CDREG32(0x801fef70));
  cd_seek_state_machine(CDREG32(0x801fef74));
  cd_seek_state_machine(CDREG32(0x801fef78));
  cd_seek_state_machine(CDREG32(0x801fef7c));
  CDREG32(0x801fefac) = 0;
  CDREG32(0x801fefa8) = CDREG32(0x801fefac);
  CDREG32(0x801fefa4) = CDREG32(0x801fefa8);
  CDREG32(0x801fefa0) = CDREG32(0x801fefa4);
  return (uint32_t)((int32_t)s0 >> 1);
}

/* cd_queue_a_read — queue-A packed read (no side effects):
 * return q90 + q94*2 + q98*4 + q9c*8. Disasm 0x801a1148-0x801a1180. */
uint32_t cd_queue_a_read(void)
{
  return CDREG32(0x801fef90)
       + (CDREG32(0x801fef94) << 1)
       + (CDREG32(0x801fef98) << 2)
       + (CDREG32(0x801fef9c) << 3);
}

/* ======== 0x80199440 ======== */
/* CD directory-entry scan / command issuer (game-side, 0x80199xxx
 * cluster). Walks the 0x20-stride entry table @*(u32*)0x801fc6c0 at
 * index *(u32*)0x801fc6a4, validates tag/sequence fields, then issues
 * a 0x1f8-int8_t DMA-style command through either the async path
 * cd_copy_u32 or the job path cd_chan_cmd_submit (7 args, last three on
 * the guest stack). Status codes go to *(u32*)0x801bcc88; error
 * retries bump *(u32*)0x801fc6a0 when mode flag 0x801fc6b0 is set.
 * Disasm 0x80199440-0x80199d58. */
extern uint32_t cd_copy_u32(uint32_t,uint32_t,uint32_t,uint32_t);
extern uint32_t cd_chan_cmd_submit(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t,uint32_t,uint32_t);
extern void cd_dir_entry_finalize(void);

#define CD_ENT16(o)  (*(uint16_t *)guest_at(DB32(0x801feb18) + (o)))

void cd_mc_state_machine(void)
{
  CDFN(CDFN_MC_SM,0);
  /* Delegate the whole body to generated MIPS (0x80199440). The clean-C
   * port cannot reserve the guest stack frame the cd_result_wait_b out
   * buffer needs: it passes guest_sp()-0x10 as a1, but with no
   * sp-=0x40 prologue that address lands inside the callee's own frame
   * (wait_b saves s6/s7 at sp+0x30/0x34), so the 8-int8_t response copy
   * collides with saved registers and the read-back sees stack garbage
   * (bit4 set -> status 3 livelock). Generated code reserves the real
   * frame, placing the out buffer above the callee frame. Same contract
   * as cd_result_wait_b/cd_file_read_seq. */
  call_guest4(0x80199440u,0,0,0,0);
  return;
}

#undef CD_ENT16

/* ======== 0x801a12dc ======== */
/* Device-context status map. ctx = (*(*(u32*)0x801be194))() guest fn
 * ptr via call_guest. Early-out conditions on ctx+0x37/38/39,
 * *(u32*)(ctx+0x30) deref int8_t, and self-link ctx+0x10==ctx. Returns
 * mapped u8@ctx+0x49: sel==3 or sel==2 -> 1; sel==6 -> 4; else raw.
 * Disasm 0x801a12dc-0x801a13a4. */
uint32_t call_guest(uint32_t, uint32_t, uint32_t);
static uint8_t *ctx_194_get(void)
{
  uint32_t fn = *(uint32_t *)guest_at(0x801be194);
  return (uint8_t *)guest_at(call_guest(fn, 0, 0));
}
/* cd_ctx_status_map — map selector sel through the ctx record (ctx_194_get) to a status code; guest 0x801a12dc. */
int32_t cd_ctx_status_map(int32_t sel)
{
  uint8_t *v1 = ctx_194_get();
  uint8_t a0;
  if (v1[0x37] != 0 || v1[0x38] != 0)
    goto map;
  if (*(uint32_t *)(v1 + 0x10) != guest_of(v1) ||
      v1[0x39] != 0)
    goto map;
  /* ctx+0x30 is a 4-int8_t guest pointer cell — read the u32 guest address
   * and resolve it; dereferencing the cell value as a host pointer reads
   * guest address bits as a native address. */
  if (*(uint8_t *)guest_at(*(uint32_t *)(v1 + 0x30)) != 0)
    goto map;
  return v1[0x49];
map:
  a0 = v1[0x49];
  if (sel == 3)
    return 1;
  if (sel < 4) {
    if (sel == 2)
      return 1;
    return a0;
  }
  if (sel == 6)
    return 4;
  return a0;
}

/* ======== 0x801a13a8 ======== */
/* Context field getter. sel: 3->u8@e4, 1->u8@e8, 2->lhu@e6,
 * 4->(idx<0 ? u8@e3 : (idx<u8@e3 ? lhu@(*(u32*)ctx + idx*2) : 0)),
 * 0x64->u32@0x4c, else 0. Disasm 0x801a13a8-0x801a149c. */
int32_t cd_ctx_field_get(int32_t unused, int32_t sel, int32_t idx)
{
  uint8_t *v1 = ctx_194_get();
  (void)unused;
  switch (sel) {
  case 3: return v1[0xe4];
  case 1: return v1[0xe8];
  case 2: return *(uint16_t *)(v1 + 0xe6);
  case 4:
    if (idx < 0)
      return v1[0xe3];
    if (idx < v1[0xe3]) {
      /* ctx+0 is a 4-int8_t guest pointer cell holding the u16 table base —
       * read it as u32 and resolve; *(uint8_t**)v1 would read 8 bytes
       * (two guest cells) as a host pointer. */
      return *(uint16_t *)guest_at(*(uint32_t *)v1 + (uint32_t)idx * 2);
    }
    return 0;
  case 0x64:
    return *(int32_t *)(v1 + 0x4c);
  default:
    return 0;
  }
}

/* ======== 0x801a161c ======== */
/* ctx getter + tail-call cd_ctx_cmd_submit(ctx, a1). */
extern int32_t cd_ctx_cmd_submit(uint32_t, uint32_t);
void cd_ctx_cmd_fwd(int32_t unused, uint32_t a1)
{
  (void)unused;
  cd_ctx_cmd_submit(guest_of(ctx_194_get()), a1);
}

/* ======== 0x801a169c ======== */
/* ctx getter + tail-call cd_ctx_set_28_34(ctx, a1, a2). */
extern int32_t cd_ctx_set_28_34(uint32_t, uint32_t, int32_t);
void cd_ctx_set_fwd(int32_t unused, uint32_t a1, int32_t a2)
{
  (void)unused;
  cd_ctx_set_28_34(guest_of(ctx_194_get()), a1, a2);
}

/* ======== batch33 ======== */

/* cd_strcmp — strcmp: NULL/equality guards then signed-int8_t compare
 * loop; NUL test is unsigned. Args are HOST pointers (callers pass
 * guest_at results / stack bufs). Disasm 0x801984a0-0x80198500. */
int32_t cd_strcmp(const void *pa0,const void *pa1)
{
  const int8_t *a = (const int8_t *)pa0;
  const int8_t *b = (const int8_t *)pa1;
  int8_t ca, cb;
  if (pa0 == pa1) return 0;
  if (pa0 == NULL) return -1;
  if (pa1 == NULL) return 1;
  for (;;) {
    ca = *a;
    cb = *b;
    if (ca != cb) return (int32_t)ca - (int32_t)cb;
    if (*(const uint8_t *)a == 0) return 0;
    a++;
    b++;
  }
}

/* cd_dir_entry_finalize — dir-entry finalize: entry = table@0x801fc6c0 +
 * idx@0x801fc6a8 *0x20; u16@+0 = 2; u32@+0x1c -> 0x801feb00;
 * u32@+8 -> 0x801feb04; @0x801fc6a8 = @0x801fc6a4; jalr fn
 * @0x801feb10 (a0 = fn ptr itself) when nonzero; @0x801fc69c = 0.
 * Disasm 0x801990f0-0x80199178. */
void cd_dir_entry_finalize(void)
{
  uint32_t idx = DB32(0x801fc6a8);
  uint8_t *e = (uint8_t *)guest_at(DB32(0x801fc6c0) + (idx << 5));
  uint32_t t, fn;
  *(uint16_t *)e = 2;
  memcpy(&t,e + 0x1c,4);                     /* lwl/lwr unaligned u32 */
  *(uint32_t *)guest_at(0x801feb00) = t;
  *(uint32_t *)guest_at(0x801feb04) = *(uint32_t *)(e + 8);
  feb00_note(1, t);
  DB32(0x801fc6a8) = DB32(0x801fc6a4);        /* delay slot of beqz */
  fn = DB32(0x801feb10);
  if (fn != 0)
    call_guest(fn,fn,0);                    /* jalr $a0: a0 = fn ptr */
  DB32(0x801fc69c) = 0;
}

/* cd_copy_u32 — copy a2 u32 words a1 -> a0 (guest addrs); a3 unused.
 * Disasm 0x80199d5c-0x80199d84. */
uint32_t cd_copy_u32(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3)
{
  uint32_t *d = (uint32_t *)guest_at(a0);
  uint32_t *s = (uint32_t *)guest_at(a1);
  uint32_t i;
  (void)a3;
  for (i = 0; i < a2; i++) *d++ = *s++;
  return 0;
}

/* cd_mmio_strobe_20 / cd_mmio_strobe_22 — MMIO command strobe: RMW the u32
 * register pointed to by cell 0x801bdec4: (*p & 0xf0ffffff) | op,
 * op = 0x20000000 / 0x22000000. Disasm 0x8019be28-0x8019be78. */
void cd_mmio_strobe_20(void)
{
  uint32_t a = CDREG32(0x801bdec4);   /* -> 0x1F801014 (MMIO) */
  br2_st32(a,(br2_ld32(a) & 0xf0ffffffu) | 0x20000000u);
}
/* cd_mmio_strobe_22 — same register-cell RMW as cd_mmio_strobe_20 with op 0x22000000; guest 0x8019be50. */
void cd_mmio_strobe_22(void)
{
  uint32_t a = CDREG32(0x801bdec4);
  br2_st32(a,(br2_ld32(a) & 0xf0ffffffu) | 0x22000000u);
}

/* ---- BIOS svc trampolines used by cd_chan_cmd_pump ----
 * jr $t2=0xb0 with t1 = svc id; a0-a3 pass through. Disasm verified:
 * 0x801a0600->0x4e, 0x801a0610->0x50, 0x801a09e0->0x4f, 0x801a09f0->0x5c. */
uint32_t cd_svc_4e(uint32_t a0,uint32_t a1)
{ return bios_svc(0xb0,0x4e,a0,a1); }
uint32_t cd_svc_50(uint32_t a0,uint32_t a1)
{ return bios_svc(0xb0,0x50,a0,a1); }
uint32_t cd_svc_4f(uint32_t a0,uint32_t a1)
{ return bios_svc(0xb0,0x4f,a0,a1); }
uint32_t cd_svc_5c(uint32_t a0,uint32_t a1)
{ return bios_svc(0xb0,0x5c,a0,a1); }

/* cd_seek_state_machine — 0x8019d5b0 is a leaf B0 trampoline
 * (`li t1,0x0B; jr 0xB0`) i.e. kernel TestEvent(a0).  bind.c's weak
 * fallback returned 0, which made every `while(v0==0)` seek/state retry
 * loop in cd_seek_step / cd_queue_*_status / cd_pending_args_pump spin
 * forever on a "not ready" that was never really queried.  Route the real
 * service like the other cd_svc_* wrappers. */
int32_t cd_seek_state_machine(uint32_t a0)
{ return (int32_t)bios_svc(0xb0,0x0b,a0,0); }

/* cd_chan_cmd_submit — 7-arg channel command submit. a0 = channel slot,
 * entries are the DMA channel regs at 0x1f801080 + a0*0x10
 * (`lui $v0,0x1f80; addu; lw/sw 0x108x` in 0x80199d88):
 *   +0 = a1 (MADR src ptr), +4 = (a2<<16)|a3 (BCR), +8 = stack arg5 (CHCR).
 * CHCR bit 0x10000000 busy -> bounded retry (one printf
 * 0x80125b18). Flag int8_t @*(*(u32*)0x801bcc5c)+2 bit (1<<a0) set when
 * low int8_t of stack arg6 (a5) == 1 else cleared. Control u32
 * @*(*(u32*)0x801bcc58) |= 1<<(a0*4+3). Then waits bit 0x40 of int8_t
 * @*(*(u32*)0x801bcc40) before storing a4 -> CHCR (starts the DMA).
 * Returns the CHCR readback.
 * Stack args: a4 = caller sp+0x10, a5 = +0x14 (lbu), a6 = +0x18 unused.
 * Disasm 0x80199d88-0x80199f2c. */
uint32_t cd_chan_cmd_submit(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3,
                      uint32_t a4,uint32_t a5,uint32_t a6)
{
  /* 0x1F801080 + ch*0x10 is the DMA channel register block, not a RAM table:
   * +0 MADR, +4 BCR, +8 CHCR (bit 0x10000000 = DMA busy). All three fields are
   * real MMIO — route through psx_read/write_word so the runtime's DMA engine
   * sees them. Disasm 0x80199d88-0x80199f2c. */
  uint32_t s0 = a0;
  uint32_t base = 0x1f801080u + (s0 << 4);
  uint32_t retry = 0;
  uint32_t fla, fpa;
  (void)a6;

  if (psx_read_word(base + 8) & 0x10000000u) {
    for (;;) {
      if (retry == 1) {
        printf_forward((const char *)guest_at(0x80125b18));
        break;
      }
      if ((psx_read_word(base + 8) & 0x10000000u) == 0) break;
      retry++;
    }
  }

  fla = DB32(0x801bcc5c);
  if ((a5 & 0xff) == 1)
    br2_st8(fla + 2, (uint8_t)(br2_ld8(fla + 2) | (1u << (s0 & 31))));
  else
    br2_st8(fla + 2, (uint8_t)(br2_ld8(fla + 2) & ~(1u << (s0 & 31))));

  {
    uint32_t ctl = DB32(0x801bcc58);
    br2_st32(ctl, br2_ld32(ctl) | (1u << ((s0 * 4 + 3) & 31)));
  }

  psx_write_word(base,     a1);                          /* MADR */
  psx_write_word(base + 4, (a2 << 16) | a3);             /* BCR  */

  fpa = DB32(0x801bcc40);
  while ((br2_ld8(fpa) & 0x40) == 0) {
    if (br2_bail_pending()) return 0;
    br2_irq_poll();
  }

  psx_write_word(base + 8, a4);                          /* CHCR: start */
  return psx_read_word(base + 8);                        /* CHCR readback */
}

/* cd_chan_cmd_pump — CD command pump, up to 8 retries. a0 = channel,
 * a1 = sub-command index, a2 = guest ptr to 0x80-int8_t request whose
 * int8_t 0x7f receives the XOR checksum of bytes 0..0x7e.
 * Per retry: svc B0/0x50, svc B0/0x4e (must ret 1), poll B0/0x5c
 * (a0 = a0>>4 sra) until bit0, memset work buf, svc B0/0x50 again,
 * svc B0/0x4f (ret!=1 -> printf 0x80125f78, skip poll), optional
 * B0/0x5c poll, then XOR-check response buf int8_t 0x7f vs request.
 * Guest work buf = sp+0x10 (0x80B) -> guest_sp()-0xa0 (frame 0xb0);
 * `addiu $s2,$sp,0x10` / `addiu $a0,$sp,0x10` in 0x801a0660.
 * svc50 carry args: iter0 a0=-1 (post-checksum), later a0=buf+0x7f;
 * a1 = running xor. Disasm 0x801a0660-0x801a07a0. */
int32_t cd_chan_cmd_pump(uint32_t a0,uint32_t a1,uint32_t a2)
{
  /* IRQ/coroutine-boundary function: clean-C body cannot supply the
   * live guest register file the deferred CD coroutine needs, and its
   * MMIO busy-waits lack per-block interrupt checks. Delegate to the
   * generated MIPS (behavioral oracle) — same contract as the other
   * read-path functions already delegated. */
  return (int32_t)call_guest4(0x801a0660u,a0, a1, a2, 0);
}

/* ======== batch34 ======== */

/* cd_ctx_set_28_34 — ctx field setter: u32 a1 -> ctx+0x28, u8 a2 -> ctx+0x34.
 * a0 is a GUEST ctx addr. Leaf; jr ra delay slot is the second store.
 * $v0 is never written, so the return value is whatever the caller left
 * in it — callers ignore it; 0 returned here.
 * Disasm 0x801a24c0-0x801a24c8. */
int32_t cd_ctx_set_28_34(uint32_t a0, uint32_t a1, int32_t a2)
{
  uint8_t *ctx = (uint8_t *)guest_at(a0);
  *(uint32_t *)(ctx + 0x28) = a1;
  ctx[0x34] = (uint8_t)a2;
  return 0;
}

/* cd_ctx_cmd_submit — ctx command submit. Calls guard fn *0x801be19c
 * (a0 = ctx guest addr, a1 = arg). If it returns nonzero, that value
 * is returned unchanged. If zero: ctx[0x46] = 1, ctx+0x14 = guest fn
 * 0x801a2de4, ctx+0x20 = a1, ctx+0x18 = guest fn 0x801a2e00 (the two
 * state-machine callbacks), and 1 is returned.
 * Disasm 0x801a2d7c-0x801a2de0. */
int32_t cd_ctx_cmd_submit(uint32_t a0, uint32_t a1)
{
  uint8_t *ctx = (uint8_t *)guest_at(a0);
  uint32_t fn = DB32(0x801be79c);
  uint32_t r = call_guest(fn, a0, a1);
  if (r != 0)
    return (int32_t)r;
  ctx[0x46] = 1;
  *(uint32_t *)(ctx + 0x14) = 0x801a2de4;
  *(uint32_t *)(ctx + 0x20) = a1;
  *(uint32_t *)(ctx + 0x18) = 0x801a2e00;
  return 1;
}

/* ======== 0x801a2de4 ======== */
/* ctx callback: ctx+0x37=0x4d, +0x36=6, +0x2c=ctx[0x20] (delay slot). */
void cd_ctx_cb_4d(uint32_t a0g)
{
  uint8_t *ctx = (uint8_t *)guest_at(a0g);
  uint32_t v1 = *(uint32_t *)(ctx + 0x20);
  ctx[0x37] = 0x4d;
  ctx[0x36] = 6;
  *(uint32_t *)(ctx + 0x2c) = v1;
}

/* ======== 0x801a2e00 ======== */
/* ctx callback: per t0 in [0,ctx[0xe9]): count matches of int8_t t0 in
 * first 6 bytes at ctx[0x20] -> a3; t1 = int8_t@(ctx[1] + t0*5 + 2)
 * (0->1); second 6-int8_t scan writes ctx[0x5d+i] = 0xff while a3<t1
 * else 0, decrementing a3 per hit. Tail ctx[0x46]=0xfe, ret 0.
 * Disasm 0x801a2e00-0x801a2ec4. */
int32_t cd_ctx_cb_scan(uint32_t a0g)
{
  uint8_t *ctx = (uint8_t *)guest_at(a0g);
  int32_t t0, t2;
  if (ctx[0xe9] == 0)
    goto done;
  t0 = 0; t2 = 0;
  do {
    uint8_t *a1 = (uint8_t *)guest_at(*(uint32_t *)(ctx + 0x20));
    int32_t a3 = 0, v1;
    int32_t t1;
    for (v1 = 5; v1 >= 0; v1--) {
      if (*a1 == (uint8_t)t0)
        a3++;
      a1++;
    }
    t1 = *(uint8_t *)((uint8_t *)guest_at(*(uint32_t *)(ctx + 4)) + t2 + 2);
    if (t1 == 0)
      t1 = 1;
    {
      uint8_t *a2 = ctx;
      a1 = (uint8_t *)guest_at(*(uint32_t *)(ctx + 0x20));
      for (v1 = 0; v1 < 6; v1++) {
        if (*a1 == (uint8_t)t0) {
          if (a3 < t1) {
            a2[0x5d] = 0xff;
            a3--;
          } else {
            a2[0x5d] = 0;
          }
        } else {
          /* int8_t mismatch: still writes? check disasm: sb only on match paths */
        }
        a1++;
        a2++;
      }
    }
    t0++;
    t2 += 5;
  } while (t0 < ctx[0xe9]);
done:
  ctx[0x46] = 0xfe;
  return 0;
}

/* ======== batch_mass1: small wrappers / trampolines ======== */

extern uint32_t cd_strncmp(uint32_t,uint32_t,uint32_t);
extern void FUN_8019a364(void);
extern void FUN_8019e600(void);
extern uint8_t *guest_memcpy(uint32_t *,uint32_t *,int);
int cd_queue_push(uint32_t,uint32_t,uint32_t,uint32_t);
/* cd_chan_reset_seq — kobj mcalls 0x8019A364/0x8019A310, cd_crit_c003, clear flag @0x801BCC90. */
void cd_chan_reset_seq(void);
/* cd_crit_c003 — DI; bios_tramp_c003(1,0x801FEB40); EI; return 1. */
int cd_crit_c003(void);
/* ret_zero_f — tail pair: FUN_8019E420 then FUN_8019E600 (CPS continuation chain). */
void ret_zero_f(void);

/* 0x80195bd0 — BIOS vector trampoline: jr $t2=0xb0, t1=7; a0/a1 fwd. */
uint32_t cd_bios_svc_07(uint32_t a0,uint32_t a1)
{
  static int cdev_dbg = -1;
  if (cdev_dbg < 0) { const char *e = getenv("PSX_CDEV"); cdev_dbg = (e && e[0] && e[0] != '0') ? 1 : 0; }
  if (cdev_dbg) {
    extern uint64_t psx_cycle_count;
    fprintf(stderr, "[cdev] DeliverEvent a0=0x%08X a1=0x%08X cyc=%llu\n",
            a0, a1, (unsigned long long)psx_cycle_count);
  }
  return bios_svc(0xb0,7,a0,a1);
}

/* 0x80199fd0 — BIOS vector trampoline: jr $t2=0xb0, t1=0x16. */
uint32_t cd_bios_svc_16(uint32_t a0,uint32_t a1)
{ return bios_svc(0xb0,0x16,a0,a1); }

/* ======== 0x80197e08 ======== */

/* cd_strncmp12_eq — true when cd_strncmp(a0,a1,12)==0 (12-byte name compare). */
int cd_strncmp12_eq(uint32_t a0,uint32_t a1)
{
  return cd_strncmp(a0,a1,0xc) == 0;
}

/* ======== 0x80198fe4 ======== */

/* cd_mc_sm_wrap — forward to cd_mc_state_machine (memcard state step). */
void cd_mc_sm_wrap(void)
{
  cd_mc_state_machine();
}

/* ======== 0x80199fa4 ======== */

/* cd_chan_reset_fwd — forward to cd_chan_reset_seq. */
void cd_chan_reset_fwd(void)
{
  cd_chan_reset_seq();
}

/* ======== 0x8019e300 ======== */

/* cd_reset_chain_a — ret_zero_f() then return 0 (CPS chain tail). */
int32_t cd_reset_chain_a(void)
{
  ret_zero_f();
  return 0;
}

/* ======== 0x80192ef0 ======== */

/* cd_queue_push_wrap — cd_queue_push(a0,a1,0,a2) with mode slot fixed 0. */
int cd_queue_push_wrap(uint32_t a0,uint32_t a1,uint32_t a2)
{
  return cd_queue_push(a0,a1,0,a2);
}

/* ======== 0x8019d1f0 ======== */

/* cd_tbl8_dispatch9 — cd_dispatch_tbl8(9,a0): table-8 slot 9 dispatch. */
int cd_tbl8_dispatch9(uint32_t a0)
{
  return cd_dispatch_tbl8(9,a0);
}

/* ======== 0x80193ce0 ======== */

/* cd_drain_dispatch — cd_dispatch_tbl4(2,0x801931C4): drain-table slot 2 -> 0x801931C4. */
int cd_drain_dispatch(void)
{
  return cd_dispatch_tbl4(2,0x801931c4);
}

/* ======== 0x80195b4c ======== */

/* cd_bios_cmd20 — BIOS svc 0x07 cmd 0x20 on object 0xF0000003. */
uint32_t cd_bios_cmd20(void)
{
  return cd_bios_svc_07(0xf0000003,0x20);
}

/* ======== 0x80195b74 ======== */

/* cd_bios_cmd40 — BIOS svc 0x07 cmd 0x40 on object 0xF0000003. */
uint32_t cd_bios_cmd40(void)
{
  return cd_bios_svc_07(0xf0000003,0x40);
}

/* ======== 0x80195b9c ======== */

/* cd_bios_cmd40_b — sibling entry, same svc 0x07 cmd 0x40 call. */
uint32_t cd_bios_cmd40_b(void)
{
  return cd_bios_svc_07(0xf0000003,0x40);
}

/* ======== 0x80199f7c ======== */

/* cd_bios16_status — B0/0x16 status query then return ~*(u32*)0x801FEB28. */
uint32_t cd_bios16_status(void)
{
  cd_bios_svc_16(0,0);
  return ~DB32(0x801feb28);
}

/* ======== 0x8019a15c ======== */

/* cd_chan_reset_seq — kobj mcalls 0x8019A364/0x8019A310, cd_crit_c003, clear flag @0x801BCC90. */
void cd_chan_reset_seq(void)
{
  FUN_8019a364();
  bios_tramp_b014(0,0,0,0);
  cd_crit_c003();
  DB32(0x801bcc90) = 0;
}

/* ======== 0x8019a20c ======== */

int cd_crit_c003(void)
{
  FUN_8018e300();
  bios_tramp_c003(1,0x801feb40);
  FUN_8018e310(0);
  return 1;
}

/* ======== 0x8019a12c ======== */

int cd_chan_init_trio(void)
{
  /* Faithful port of generated 0x8019A12C (48B / 4 blocks, decoded
   * insn-by-insn against generated/SLES_017.22_full_46.c:21253):
   *   frame 0x18 (sp-=0x18, ra saved at sp+0x10 — dead slot, kept via
   *   br2_guest_frame so nested generated calls see the same $sp)
   *   jal 0x8019A300  (B0/0x13 CD channel init tramp)   ra=0x8019A13C
   *   a0=0 (delay slot), jal bios_table_tramp 0x80199FE0 (B0/0x5b)
   *                                                    ra=0x8019A144
   *   jal 0x8019A350  (kobj method tail-call via *0x801FEB58)
   *                                                    ra=0x8019A14C
   *   epilogue: lw ra,16(sp); sp+=0x18; v0=1; jr ra.
   * All three jals are BIOS-adjacent thin tramps — exactly the class the
   * delegation rule permits as jal TARGETS — driven through cd_jal so
   * a0-a3 keep callee leftovers like the guest. c784bf1's regression was
   * a reconstructed guest frame around the BIOS boundary; this port has
   * no BIOS/kernel work in its OWN flow, only trampoline jals. */
  uint32_t fr = br2_guest_frame(0x18);
  CPUState *cpu = clean_cpu_state();
  (void)fr;
  (void)cd_jal(0x8019A300u, 0x8019A13Cu);          /* jal B0/0x13 tramp */
  if (br2_bail_pending()) { br2_guest_frame_abandon(); return 0; }
  cpu->gpr[4] = 0;                                 /* move a0,$0 delay slot */
  (void)cd_jal(0x80199FE0u, 0x8019A144u);          /* jal bios_table_tramp */
  if (br2_bail_pending()) { br2_guest_frame_abandon(); return 0; }
  (void)cd_jal(0x8019A350u, 0x8019A14Cu);          /* jal kobj method tramp */
  if (br2_bail_pending()) { br2_guest_frame_abandon(); return 0; }
  br2_guest_frame_free();
  return 1;
}

/* ======== 0x80191978 ======== */

/* cd_blk5c_copy — copy the 0x5c-byte CD status block @0x801B8330 to a0; returns a0. */
uint32_t cd_blk5c_copy(uint32_t a0)
{
  guest_memcpy((uint32_t *)(uintptr_t)a0,(uint32_t *)(uintptr_t)0x801b8330,0x5c);
  return a0;
}

/* ======== 0x80191ea4 ======== */

/* cd_blk14_copy — copy the 0x14-byte CD sub-block @0x801B838C to a0; returns a0. */
uint32_t cd_blk14_copy(uint32_t a0)
{
  guest_memcpy((uint32_t *)(uintptr_t)a0,(uint32_t *)(uintptr_t)0x801b838c,0x14);
  return a0;
}

/* ======== 0x801936b0 ======== */

/* cd_timeout_arm — arm the CD timeout: deadline @0x801B845C = timer+0xF0, counter @0x801B8460 = 0. */
void cd_timeout_arm(void)
{
  DB32(0x801b845c) = cd_timer_read(0xffffffff) + 0xf0;
  DB32(0x801b8460) = 0;
}

/* ======== 0x8019e3d4 ======== */

void ret_zero_f(void)
{
  bios_tramp_b04c(0,0,0,0);
  FUN_8019e600();
}

/* ======== 0x8019a710 ======== */

/* cd_ind_dispatch_14 — indirect dispatch: call *(u32*)(*0x801BDDA0 + 0x14) with (4, a0). */
uint32_t cd_ind_dispatch_14(uint32_t a0)
{
  uint32_t tbl = CDB32G(0x801bdda0);
  uint32_t fn = *(uint32_t *)guest_at(tbl + 0x14);
  return call_guest(fn,4,a0);
}

/* ======== batch_mass2 ======== */

extern int gpu_coord_pack(int,int);
extern void cd_guest_zero32_c(uint32_t,uint32_t);
extern void cd_guest_zero32_b(uint32_t,uint32_t);
extern void FUN_8019a0ddc(void);
extern void cd_file_open(int32_t,uint8_t *);
extern int cd_stamp_to_sector(const void *);

/* 0x8019a774 — indirect dispatch tbl+0x10 (head loads *0x801bdda0). */
uint32_t cd_ind_dispatch_10(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3)
{
  uint32_t tbl = CDB32G(0x801bdda0);
  uint32_t fn = *(uint32_t *)guest_at(tbl + 0x10);
  (void)a2; (void)a3;
  return call_guest(fn,a0,a1);
}

/* 0x8019a7a4 — indirect dispatch tbl+0x18. */
uint32_t cd_ind_dispatch_18(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3)
{
  uint32_t tbl = CDB32G(0x801bdda0);
  uint32_t fn = *(uint32_t *)guest_at(tbl + 0x18);
  (void)a2; (void)a3;
  return call_guest(fn,a0,a1);
}

/* ======== 0x8019d1b0 ======== */

/* cd_state9_store — store a0 to state cell @0x801BDEEC on change; fires cd_tbl8_dispatch9(a0); returns previous. */
uint32_t cd_state9_store(uint32_t a0)
{
  uint32_t old = DB32G(0x801bdeec);
  if (a0 != old) {
    DB32(0x801bdeec) = a0;
    cd_tbl8_dispatch9(a0);
  }
  return old;
}

/* ======== 0x80191f88 ======== */

/* cd_coord_pack_entry — pack s16 pair at a1 via gpu_coord_pack, store tag 2 + packed result +0/+4/+8 at a0. */
int cd_coord_pack_entry(uint32_t a0,uint32_t a1)
{
  int packed;
  *(uint8_t *)guest_at(a0 + 3) = 2;
  packed = gpu_coord_pack((int)*(int16_t *)guest_at(a1),
                       (int)*(int16_t *)guest_at(a1 + 2));
  *(uint32_t *)guest_at(a0 + 4) = (uint32_t)packed;
  *(uint32_t *)guest_at(a0 + 8) = 0;
  return packed;
}

/* ======== 0x8019d170 ======== */

/* cd_reg_rw_d2 — indexed register op 0xD2 on a0 (rejects a0 > 0x7FFFFFF8). */
int32_t cd_reg_rw_d2(uint32_t a0)
{
  if (a0 > 0x7ffffff8u) return 0;
  return cd_reg_indexed_rw(0xd2,a0);
}

/* ======== 0x8019e6f0 ======== */

/* cd_block7_init — wait for @0x801FEBE0 drain (IRQ poll), then cd_chan_block_init(7,0) + FUN_8019A0DDC. */
void cd_block7_init(void)
{
  while (DB32(0x801febe0) != 0) {
    if (br2_bail_pending()) return;
    br2_irq_poll();
  }
  cd_chan_block_init(7,0);
  FUN_8019a0ddc();
}

/* ======== 0x8019ef30 ======== */

/* cd_pending_release — if pending slot @0x801FEBF4 >= 0, release via FUN_801A0250 and mark -1. */
void cd_pending_release(void)
{
  int32_t v = (int32_t)DB32(0x801febf4);
  if (v >= 0) {
    bios_tramp_b036((uint32_t)v,0,0,0);
    DB32(0x801febf4) = 0xffffffff;
  }
}

/* ======== 0x8019746c ======== */

/* cd_chan_reset_b — trace CDFN_CHAN_RESET_B; clears cells @0x801BC848/4C/54/58 then tbl-c + tbl-8 dispatches. */
void cd_chan_reset_b(void)
{
  CDFN(CDFN_CHAN_RESET_B,0);
  DB32(0x801bc84c) = 0;
  DB32(0x801bc848) = 0;
  DB32(0x801bc858) = 0;
  DB32(0x801bc854) = 0;
  cd_dispatch_tblc();
  cd_dispatch_tbl8(2,0x801979f8);
}

/* ======== 0x8019af30 ======== */

/* cd_init_stage_b — zero handler-table words @0x801BDDF4 (8), clear @0x801BDDF0, tbl8 dispatch slot 3 -> 0x8019AF7C. */
uint32_t cd_init_stage_b(void)
{
  /* Generated zeroes 8 handler-table words at 0x801BDDF4 (0x801C0000-8716),
   * indices 0-7. 0x801BDDFC is index 2 — starting there would skip slots
   * 0-1 and wrongly clear 0x801BDE14/0x801BDE18 past the table end. */
  cd_guest_zero32_c(0x801bddf4,8);
  br2_st32p(0x801bddf0, 0);
  cd_dispatch_tbl8(3,0x8019af7c);
  return 0x8019b0fc;
}

/* ======== 0x8019b0fc ======== */

/* cd_chan_cb_set — DMA-channel done-callback register/clear.
 * a0 = slot index, a1 = handler guest address (0 = clear).
 *
 * Faithful port of generated 0x8019B0FC-0x8019B1A4 (5 blocks, no calls,
 * no loops — a leaf, so the per-block psx_check_interrupts_at has no
 * observable effect):
 *   entry = 0x801BDDF4 + idx*4   (handler-table word; generated
 *           0x8019B100-0x8019B10C: lui 0x801C + addiu -8716 =
 *           0x801BDDF4, + a0*4 — same base as the cb7/cb8 dispatchers)
 *   old   = *entry
 *   if (a1 == old) return old;            (no DICR touch on no-op)
 *   dicr  = *(u32*)0x801BDDF0             (pointer cell -> 0x1F8010F4
 *                                          DICR; MMIO, so br2_ld/st)
 *   flag  = *dicr
 *   *entry = a1
 *   flag  = (flag & 0x00FFFFFF) | 0x00800000
 *   a1 != 0 -> flag |=  1 << ((idx+16) & 31)
 *   a1 == 0 -> flag &= ~(1 << ((idx+16) & 31))
 *   *dicr = flag
 *   return old                            (v0 = old in every path,
 *                                            incl. the equal early-out)
 * Disasm verified against generated/SLES_017.22_full_46.c: beq a1,old
 * @0x8019B118, beq a1,zero @0x8019B120, set path 0x8019B128-0x8019B160,
 * clear path 0x8019B164-0x8019B19C, jr ra @0x8019B1A0. */
uint32_t cd_chan_cb_set(uint32_t idx,uint32_t handler)
{
  uint32_t entry = 0x801bddf4u + idx * 4u;
  uint32_t old   = br2_ld32(entry);
  uint32_t dicr, flag, bit;
  if (handler == old) return old;
  dicr = br2_ld32(0x801bddf0u);            /* cell -> DICR addr (MMIO) */
  flag = br2_ld32(dicr);
  bit  = 1u << ((idx + 16u) & 31u);
  br2_st32(entry, handler);
  flag = (flag & 0x00ffffffu) | 0x00800000u;
  if (handler != 0) flag |= bit; else flag &= ~bit;
  br2_st32(dicr, flag);
  return old;
}

/* ======== 0x8019ae10 ======== */

/* cd_init_stage_a — init cells @0x801BDDE4/0x801BDDE0, zero32 @0x801BDDC0 (8), tbl8 dispatch slot 0 -> 0x8019AE68. */
uint32_t cd_init_stage_a(void)
{
  br2_st32p(0x801bdde4, 0x100);
  DB32(0x801bdde0) = 0;
  cd_guest_zero32_b(0x801bddc0,8);
  cd_dispatch_tbl8(0,0x8019ae68);
  return 0x8019aed4;
}

/* ======== batch_mass3 ======== */

extern uint32_t gpu_invoke(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
/* cd_gpu_call — table-driven gpu_invoke wrapper: fn=*(tbl+off) via *0x801B8318. */
static uint32_t cd_gpu_call(uint32_t off,uint32_t a0,uint32_t a1,
                                uint32_t a2,uint32_t a3)
{
  uint32_t *t = (uint32_t *)guest_at(*(uint32_t *)guest_at(0x801b8318));
  return gpu_invoke(t[off >> 2],a0,a1,a2,a3);
}
extern void gpu_rect_bounds_dbg(uint32_t,short *);
extern int cd_pump_watchdog(void);
extern uint32_t cd_dma_xfer_start(uint32_t,uint32_t,uint32_t);

/* ======== 0x80191714 ======== */

/* cd_gpu_callpair_a — two gpu_call steps: (0x3C,0,a1,a2,a3) then (0x14,a0+4,byte[a0+3],a2,a3). */
void cd_gpu_callpair_a(uint32_t a0,uint32_t a1,uint32_t a2,
                  uint32_t a3)
{
  uint32_t s1 = DB8(a0 + 3);
  cd_gpu_call(0x3c,0,a1,a2,a3);
  cd_gpu_call(0x14,a0 + 4,s1,a2,a3);
}

/* ======== 0x80191084 ======== */

/* cd_gpu_defer_set — swap defer cell @0x801B832C; if trace>=2 fire cb @0x801B831C(0x80125484,a0); ret old. */
uint32_t cd_gpu_defer_set(uint32_t a0)
{
  uint32_t old = DB32(0x801b832c);
  if (DB8(0x801b8322) >= 2) {
    call_guest(DB32G(0x801b831c),0x80125484,a0);
  }
  DB32(0x801b832c) = a0;
  return old;
}

/* ======== 0x80191488 ======== */

/* cd_gpu_call_8 — gpu_rect_bounds_dbg(0x80125510,a0); gpu_call(8,*(tbl+0x1C),a0,8,a1). */
void cd_gpu_call_8(uint32_t a0,uint32_t a1)
{
  uint32_t obj;
  gpu_rect_bounds_dbg(0x80125510,(short *)(uintptr_t)a0);
  obj = DB32G(0x801b8318);
  cd_gpu_call(8,*(uint32_t *)guest_at(obj + 0x1c),a0,8,a1);
}

/* ======== 0x8019a244 ======== */

/* cd_hdr_clear_fixed — clear u16 @p+0xA on block @0x801BCC94 (dead loop kept for fidelity). */
void cd_hdr_clear_fixed(void)
{
  uint32_t p = DB32G(0x801bcc94);
  int i;
  *(uint16_t *)guest_at(p + 0xa) = 0;
  for (i = 9; i >= 0; i--) { }
}

/* ======== 0x8019ae68 ======== */

/* cd_cb8_dispatch — table-8 dispatch group; faithful port of 0x8019AE68. */
void cd_cb8_dispatch(void)
{
  /* Faithful port of generated 0x8019AE68 (108B / 5 blocks, decoded
   * insn-by-insn against generated/SLES_017.22_full_46.c:31795):
   *   v0 = *(u32*)0x801BDDE0; *(u32*)0x801BDDE0 = v0 + 1   // frame tick++
   *   frame 0x20 (s0/s1/ra spills — dead slots, kept via br2_guest_frame)
   *   s0 = 0x801BDDC0; s1 = 0
   *   loop i = 0..7:  fn = *(u32*)(0x801BDDC0 + i*4)
   *     if (fn != 0) jalr $ra,$v0  — ra = 0x8019AEB0, a0-a3 = live
   *       leftovers (first call sees this fn's incoming regs, later calls
   *       see the previous callee's leftovers — cd_jal preserves both).
   *   exit: v0 = 0 (the slti s1,8 result), epilogue jr ra.
   * The callback table entries are runtime-installed guest fn ptrs, so the
   * jalr goes through cd_jal (clean_try_dispatch first, generated
   * fallback) — same contract as bind_jal, minus the $t0 write since the
   * guest target arrives in $v0 (`jalr $ra,$v0`, not `jalr $t0`). */
  uint32_t fr = br2_guest_frame(0x20);
  uint32_t i, tbl;
  (void)fr;
  br2_st32(0x801BDDE0u, br2_ld32(0x801BDDE0u) + 1u);
  i = 0;
  tbl = 0x801BDDC0u;
  for (;;) {
    uint32_t fn = br2_ld32(tbl);
    if (fn != 0) {
      (void)cd_jal(fn, 0x8019AEB0u);
      if (br2_bail_pending()) { br2_guest_frame_abandon(); return; }
    }
    i++;
    tbl += 4u;
    if (i >= 8u) break;
  }
  clean_cpu_state()->gpr[2] = 0;   /* exit-path $v0 = slti(s1,8) = 0 */
  br2_guest_frame_free();
}

/* ======== 0x8019bc48 ======== */

/* cd_win_shift_op — window ops: shift u16 @0x801BDEC8 by (@0x801BDED8&31) into regs 2/0, then reg 3 with (a0,a1). */
uint32_t cd_win_shift_op(uint32_t a0,uint32_t a1)
{
  uint32_t sh = (uint32_t)DB16(0x801bdec8)
              << (DB32(0x801bded8) & 31);
  cd_reg_window_op(2,sh,0);
  cd_reg_window_op(0,sh,0);
  cd_reg_window_op(3,a0,a1);
  return a1;
}

/* ======== 0x8019842c ======== */

/* cd_file_load_sync — open file a1 into guest-frame buf, mode_flag_set(2), read_setup(a0,a2,0x80), poll; returns poll==0. */
int cd_file_load_sync(uint32_t a0,uint32_t a1,uint32_t a2)
{
  /* buf is written by cd_file_open through a host pointer, then handed
   * to cd_mode_flag_set -> generated code as a raw guest address — it
   * must live in guest-visible memory, not on the host stack. */
  uint32_t fr = br2_guest_frame(0x10);
  uint8_t *buf = (uint8_t *)guest_at(fr);
  int r;
  cd_file_open((int32_t)a1,buf);
  cd_mode_flag_set(2,(int)fr,0);
  cd_read_setup(a0,a2,0x80);
  r = cd_read_poll(0,0) == 0;
  br2_guest_frame_free();
  return r;
}

/* ======== 0x8019e730 ======== */

/* cd_job_start_e — if job block @0x801FEBE0 busy print+ret 0; else arm state cells + job_slot_alloc(0x8019E798,a0); ret 1. */
int cd_job_start_e(uint32_t a0)
{
  if ((int32_t)DB32(0x801febe0) > 0) {
    printf_forward((const char *)guest_at(0x80125ce8));
    return 0;
  }
  DB32(0x801febe0) = 1;
  DB32(0x801febe4) = 0;
  DB32(0x801febe8) = 0;
  DB32(0x801febf0) = a0;
  cd_job_slot_alloc(0x8019e798,a0);
  return 1;
}

/* ======== 0x80195c1c ======== */

/* cd_init_mode_dispatch — trace; a0==2 -> chan_reset_b; else subsystem_init, a0==1 -> reg_init_seq. */
int cd_init_mode_dispatch(uint32_t a0)
{
  CDFN(CDFN_INIT_MODE_DISPATCH,a0);
  if (a0 == 2) {
    cd_chan_reset_b();
    return 1;
  }
  if (cd_subsystem_init() != 0) return 0;
  if (a0 != 1) return 0;
  if (cd_reg_init_seq() != 0) return 0;
  return 1;
}

/* ======== 0x801915a0 ======== */

/* cd_prim_list_a — walk prim list a0 (count a1-1): clear tag byte +3, link each node to next via low-24 bits; tail-link @0x801B83E0. */
uint32_t cd_prim_list_a(uint32_t a0,uint32_t a1)
{
  uint32_t s0 = a0;
  uint32_t s1 = a1;
  uint32_t nxt, v0, v1;
  if (DB8(0x801b8322) >= 2) {
    call_guest(DB32G(0x801b831c),0x80125528,s0);
  }
  s1 = s1 - 1;
  if (s1 != 0) {
    do {
      s1 = s1 - 1;
      nxt = s0 + 4;
      *(uint8_t *)guest_at(s0 + 3) = 0;
      v0 = DB32(s0);
      v1 = nxt & 0xffffff;
      v0 = (v0 & 0xff000000) | v1;
      DB32(s0) = v0;
      if (s1 == 0) break;
      s0 = nxt;
    } while (1);
  }
  v0 = s0;
  DB32(0x801b83e0) = (0x801b83cc & 0xffffff) | 0x4000000;
  *(uint32_t *)guest_at(v0) = 0x801b83e0 & 0xffffff;
  return v0;
}

/* ======== 0x80193be4 ======== */

/* cd_gpu_call_18 — poll GPU status @0x801B8434 until ready (watchdog); dispatch tbl4(2,0x80193CE0) then gpu_call(0x18,a0). */
int32_t cd_gpu_call_18(uint32_t a0)
{
  for (;;) {
    if (br2_bail_pending()) return -1;
    uint32_t st = br2_ld32(DB32G(0x801b8434));
    uint32_t o;
    int poll;
    if (DB8(0x801b8322) >= 2 && st == 0) { }
    if ((st & 0x1000000) != 0) {
      poll = 1;
    } else {
      o = br2_ld32(DB32G(0x801b8428));
      poll = (o & 0x04000000) == 0;
    }
    if (!poll) break;
    if (cd_pump_watchdog() != 0) return -1;
  }
  cd_dispatch_tbl4(2,0x80193ce0);
  cd_gpu_call(0x18,a0,0,0,0);
  return 0;
}

/* ======== 0x8019cfc0 ======== */

/* cd_dma_xfer_mode — mode dispatch: 5->xfer(a0,0,0x20), 6->xfer(a0+0x800,0x100,0x20), else xfer(a0,0,0x40); ret status bit 0x800. */
int cd_dma_xfer_mode(uint32_t a0,uint32_t a1)
{
  if (a1 == 5) {
    cd_dma_xfer_start(a0,0,0x20);
  } else if (a1 == 6) {
    cd_dma_xfer_start(a0 + 0x800,0x100,0x20);
  } else {
    cd_dma_xfer_start(a0,0,0x40);
  }
  return (br2_ld16(DB32G(0x801bdeb0) + 0x1ae) & 0x800) != 0;
}

/* ======== 0x8019aca4 ======== */

/* cd_state_blk_save — save block @0x801BCD18: if flag clear, FUN_8019AD80(+0x38), set flag, stash fields to globals, crit-enter; ret s0. */
uint32_t cd_state_blk_save(void)
{
  uint32_t s0 = 0x801bcd18;
  if (*(uint16_t *)guest_at(s0) != 0) return 0;
  bios_tramp_b019(s0 + 0x38,0,0,0);
  *(uint16_t *)guest_at(s0) = 1;
  br2_st16p(0x801bdda8, *(uint16_t *)guest_at(s0 + 0x32));
  br2_st32p(0x801bddac, *(uint32_t *)guest_at(s0 + 0x34));
  FUN_8018e310(DB32G(0x801bdda8));
  return s0;
}

/* ======== batch_mass4 ======== */

extern void cd_kobj_init_a(void);
extern void cd_kobj_init_b(void);
extern int cd_seek_state_machine(uint32_t);
extern void cd_mmio_strobe_22(void);

/* ======== 0x8019a09c ======== */

int cd_subsys_init_full(uint32_t a0,uint32_t a1,uint32_t a2,
                 uint32_t a3)
{
  /* e9fc366 clean port of 0x8019a09c regressed early boot (same root
   * cause as cd_kobj_init_a/b — inline B0 BIOS svc6 path). Delegate
   * whole body to generated MIPS. */
  return (int)call_guest4(0x8019a09cu,a0,a1,a2,a3);
}

/* ======== 0x8019ac04 ======== */

/* cd_state_blk_restore — inverse of save: restore fields, clear MMIO cells, FUN_8019AD70, clear flag; ret s0. */
uint32_t cd_state_blk_restore(void)
{
  uint32_t s0 = 0x801bcd18;
  uint32_t v0, v1, a0;
  if (*(uint16_t *)guest_at(s0) == 0) return 0;
  FUN_8018e300();
  v0 = DB32G(0x801bdda8);
  a0 = DB32G(0x801bddac);
  v1 = br2_ld16(v0);
  *(uint16_t *)guest_at(s0 + 0x32) = (uint16_t)v1;
  v1 = br2_ld32(a0);
  a0 = DB32G(0x801bdda4);
  *(uint32_t *)guest_at(s0 + 0x34) = v1;
  br2_st16(v0, 0);
  v0 = br2_ld16(v0);
  br2_st16(a0, (uint16_t)v0);
  a0 = DB32G(0x801bddac);
  v0 = br2_ld32(a0) & 0x77777777;
  br2_st32(a0, v0);
  bios_tramp_b018(a0,0,0,0);
  *(uint16_t *)guest_at(s0) = 0;
  return s0;
}

/* ======== 0x8019b89c ======== */

uint32_t cd_dma_xfer_start(uint32_t a0,uint32_t a1,uint32_t a2)
{
  /* Faithful port of generated 0x8019B89C. Straight-line MMIO sequence with
   * three calls to already-clean leaves (cd_delay_3c x2, cd_mmio_strobe_22);
   * no loops, no BIOS/kernel boundary, no gpr snapshot, no indirect dispatch.
   * Body mirrors the runtime-verified mid-entry port cd_dma_xfer_start_b
   * (0x8019B8A4), plus the prologue's blk = *0x801bdeb0 load and the
   * arg->s0/s1 renaming (a0->s1, a1->sh, a2->s0). The sll $s0,16 sits in the
   * delay slot of the second cd_delay_3c jal — before the callee runs — but
   * $s0 is callee-saved and never an arg, so placement is unobservable.
   * Returns $v0 = 1 (li v0,1 @0x8019B920 is the last v0 write). */
  uint32_t blk = DB32G(0x801bdeb0);
  uint32_t s0;
  CDMMIO16_ST(blk,0xd3,(uint16_t)a1);          /* sh a1,0x1a6(blk) */
  s0 = a2;
  cd_delay_3c();
  blk = DB32G(0x801bdeb0);
  CDMMIO16_ST(blk,0xd5,(uint16_t)(CDMMIO16(blk,0xd5) | 0x30));
  s0 <<= 16;
  cd_delay_3c();
  cd_mmio_strobe_22();
  s0 |= 0x10;
  br2_st32(DB32G(0x801bdeb4), a0);            /* sw s1,0(*0x801bdeb4) */
  br2_st32(DB32G(0x801bdeb8), s0);                 /* sw s0,0(*0x801bdeb8) */
  DB32(0x801bdf00) = 1;                            /* DMA callback-arm cell */
  br2_st32(DB32G(0x801bdebc), 0x01000200);         /* sw a0=0x01000200 */
  return 1;
}

/* ======== batch_mass5 ======== */

/* ======== 0x80190fd0 ======== */

/* cd_gpu_call_34 — if a0 != cached byte @0x801B8321: gpu_invoke(tbl+0x34), store byte, dispatch tbl4(2,0). ret old byte. */
uint32_t cd_gpu_call_34(uint32_t a0,uint32_t a1,uint32_t a2,
                      uint32_t a3)
{
  uint32_t s2 = DB8(0x801b8321);
  if (DB8(0x801b8322) >= 2) {
    call_guest(DB32G(0x801b831c),0x80125470,a0);
  }
  if (a0 != DB8(0x801b8321)) {
    uint32_t tbl = DB32G(0x801b8318);
    gpu_invoke(*(uint32_t *)guest_at(tbl + 0x34),1,a1,a2,a3);
    DB8(0x801b8321) = (uint8_t)a0;
    cd_dispatch_tbl4(2,0);
  }
  return s2;
}

/* ======== 0x80197e28 ======== */

/* cd_dir_scan — CD_newmedia directory scan: loads the PVD (sector 0x10)
 * into 0x801fe300 via cd_file_load_sync, checks the "CD001" signature at
 * buf+1 (cd_strncmp, 5 bytes), then loads the root-dir extent (unaligned
 * u32 @buf+0x8c, lwl/lwr pair) into the same buffer. It then walks ISO
 * dir records — len@+0, extent u32@+2 (lwl/lwr), id int8_t@+6, name@+8,
 * stride 8+len+(len&1), bounded by buf+0x800 and 0x80 entries — into the
 * 0x801fcd00 table (stride 0x2c: +0 entry no., +4 id/parent int8_t, +8
 * extent, +0xc NUL-terminated name). The first empty +4 field terminates
 * the table for cd_file_rec_find, and 0x801ccb70 is cleared. Debug
 * prints gated on @0x801bc850 (>0 read errors, >=2 per-entry + count).
 * Returns 1 on success, 0 on read/format error. Disasm
 * 0x80197e28-0x801980eb. */
uint32_t cd_dir_scan(uint32_t a0,uint32_t a1)
{
  uint32_t s1, lba, cur, end, i;
  (void)a0;
  (void)a1;

  /* a0=1 (mode), a1=0x10 (PVD sector), a2=0x801fe300 (dest buffer);
   * incoming a0/a1 are overwritten before any use. */
  s1 = (uint32_t)cd_file_load_sync(1,0x10,0x801fe300);
  if (s1 != 1) {
    if ((int32_t)*(uint32_t *)guest_at(0x801bc850) <= 0) return 0;
    printf_forward((const char *)guest_at(0x80125964));
    return 0;
  }
  if (cd_strncmp(0x801fe301,0x80125990,5) != 0) {
    if ((int32_t)*(uint32_t *)guest_at(0x801bc850) <= 0) return 0;
    printf_forward((const char *)guest_at(0x80125998));
    return 0;
  }
  /* root-dir extent LBA @buf+0x8c — lwl/lwr pair (addr is 4-aligned) */
  memcpy(&lba,(const void *)guest_at(0x801fe38c),4);
  if ((uint32_t)cd_file_load_sync(1,lba,0x801fe300) != s1) {
    if ((int32_t)*(uint32_t *)guest_at(0x801bc850) <= 0) return 0;
    printf_forward((const char *)guest_at(0x801259c8),lba);
    return 0;
  }
  if ((int32_t)*(uint32_t *)guest_at(0x801bc850) >= 2)
    printf_forward((const char *)guest_at(0x801259ec));

  cur = 0x801fe300;              /* s1 = s0 = dir buffer */
  end = cur + 0x800;              /* s5 = cursor bound */
  i = 0;                          /* a3 = entry index */
  while (cur < end) {
    uint32_t len = DB8(cur);
    uint32_t idx, xv, fl, nid, nm, adv;
    if (len == 0) break;
    idx = i * 0x2c;
    /* record extent u32 @cur+2 (lwl/lwr) -> table entry +8 */
    memcpy(&xv,(const void *)guest_at(cur + 2),4);
    *(uint32_t *)guest_at(0x801fcd08 + idx) = xv;
    nm = 0x801fcd0c + idx;        /* s2 = name dst (guest addr) */
    fl = DB8(cur + 6);
    nid = i + 1;                  /* s3 = entry no. */
    *(uint32_t *)guest_at(0x801fcd00 + idx) = nid;
    *(uint32_t *)guest_at(0x801fcd04 + idx) = fl;
    guest_memcpy((uint32_t *)(uintptr_t)nm,(uint32_t *)(uintptr_t)(cur + 8),(int)len);
    DB8(nm + len) = 0;            /* NUL-terminate copied name */
    adv = len + ((len & 1) + 8);  /* record stride: 8+len rounded even */
    cur += adv;
    i = nid;
    if ((int32_t)*(uint32_t *)guest_at(0x801bc850) >= 2)
      printf_forward((const char *)guest_at(0x80125a0c),xv,nid,fl,nm);
    if (i >= 0x80) goto dir_done; /* table full: skip terminator store */
  }
  if (i < 0x80)
    *(uint32_t *)guest_at(0x801fcd04 + i * 0x2c) = 0;  /* end marker */
dir_done:
  *(uint32_t *)guest_at(0x801ccb70) = 0;
  if ((int32_t)*(uint32_t *)guest_at(0x801bc850) >= 2)
    printf_forward((const char *)guest_at(0x80125a20),i);
  return 1;
}

/* ======== 0x801980ec ======== */

/* cd_file_rec_find — scan 0x80-entry table @0x801FCD00 (stride 0x2c): match entry-no a0 + cd_strcmp name; ret index+1 or -1. */
int cd_file_rec_find(uint32_t a0,uint32_t a1)
{
  uint32_t i, off = 0;
  uint32_t s2 = 0x801fcd0c;
  for (i = 0; i < 0x80; i++) {
    uint32_t v0 = *(uint32_t *)guest_at(0x801fcd04 + off);
    if (v0 == 0) return -1;
    if (v0 == a0) {
      /* a1 arrives as a raw guest address (clean_table) — cd_strcmp
       * takes host pointers, so resolve through cd_arg_ptr. */
      if (cd_strcmp(cd_arg_ptr(a1),
                       guest_at(s2)) == 0) {
        return (int)(i + 1);
      }
    }
    s2 += 0x2c;
    off += 0x2c;
  }
  return -1;
}

/* ======== 0x801914e8 ======== */

/* cd_gpu_rect_draw — validate rect dims (w/h !=0), stash params @0x801B83C0-E8, gpu_invoke(tbl+8) packet emit; ret height. */
int32_t cd_gpu_rect_draw(uint32_t a0,uint32_t a1,uint32_t a2)
{
  int32_t v0;
  uint32_t tbl;
  gpu_rect_bounds_dbg(0x8012551c,(short *)(uintptr_t)a0);
  v0 = *(int16_t *)guest_at(a0 + 4);
  if (v0 == 0) return -1;
  v0 = *(int16_t *)guest_at(a0 + 6);
  if (v0 == 0) return -1;
  *(uint32_t *)guest_at(0x801b83c0) = DB32(a0);
  *(uint32_t *)guest_at(0x801b83c4) = (a2 << 16) | (a1 & 0xffff);
  *(uint32_t *)guest_at(0x801b83c8) = DB32(a0 + 4);
  tbl = DB32G(0x801b8318);
  gpu_invoke(*(uint32_t *)guest_at(tbl + 8),
                 *(uint32_t *)guest_at(tbl + 0x18),0x801b83b8,0x14,0);
  return (int32_t)DB32(a0 + 4);
}

/* ======== batch_mass6 ======== */

/* shared shape: 0x8019ef74 (state 5, cb 0x8019f02c) and 0x8019f168
 * (state 6, cb 0x8019f220). Guard block @0x801febe0..0x801fec00. */
static int cd_submit_check(uint32_t a0,uint32_t a1,uint32_t a2,
                       uint32_t state,uint32_t cb)
{
  if ((int32_t)DB32(0x801febf4) < 0) {
    printf_forward((const char *)guest_at(0x80125e0c));
    return 0;
  }
  if ((int32_t)DB32(0x801febe0) > 0) {
    printf_forward((const char *)guest_at(0x80125ce8));
    return 0;
  }
  if ((a2 & 0x7f) != 0) {
    printf_forward((const char *)guest_at(0x80125e30));
    return 0;
  }
  if ((a1 & 0x7f) != 0) {
    printf_forward((const char *)guest_at(0x80125e5c));
    return 0;
  }
  DB32(0x801febe0) = state;
  DB32(0x801febe4) = 0;
  DB32(0x801febe8) = 0;
  DB32(0x801febf8) = a1;
  DB32(0x801fec00) = a0;
  DB32(0x801febfc) = a2;
  cd_job_slot_alloc(cb,a1);
  return 1;
}

/* ======== 0x8019ef74 ======== */

/* cd_submit5 — cd_submit_check(a0,a1,a2,state=5,cb=0x8019F02C): guarded job submit. */
int cd_submit5(uint32_t a0,uint32_t a1,uint32_t a2)
{
  return cd_submit_check(a0,a1,a2,5,0x8019f02c);
}

/* ======== 0x8019f168 ======== */

/* cd_submit6 — cd_submit_check(a0,a1,a2,state=6,cb=0x8019F220). */
int cd_submit6(uint32_t a0,uint32_t a1,uint32_t a2)
{
  return cd_submit_check(a0,a1,a2,6,0x8019f220);
}

/* ======== batch_mass7 ======== */

extern void gpu_prim_pkt_build(int,uint16_t *);
extern void cd_guest_zero32_a(uint32_t,uint32_t);
extern int cd_ctx_save(uint32_t);
extern void cd_intc_dispatch(void);

/* shared CD wait: poll **0x801b8434 bit23 / **0x801b8428 bit22 with
 * cd_pump_watchdog pump; -1 on pump failure. (0x80193be4/938c8/939b4) */
static int32_t cd_wait_stat(void)
{
  for (;;) {
    if (br2_bail_pending()) return -1;
    uint32_t st = br2_ld32(DB32G(0x801b8434));
    uint32_t o;
    int poll;
    if ((st & 0x1000000) != 0) {
      poll = 1;
    } else {
      o = br2_ld32(DB32G(0x801b8428));
      poll = (o & 0x04000000) == 0;
    }
    if (!poll) return 0;
    if (cd_pump_watchdog() != 0) return -1;
  }
}

/* ======== 0x801979f8 ======== */

/* 0x801979f8 — IRQ2-context CD response/completion dispatcher (INTC
 * handler[2] @0x801bcd1c, gated on I_STAT bit2 CDROM). Faithful port of
 * generated 0x801979f8-0x80197ACC (54 insns, decoded insn-by-insn):
 *
 *   s2 = lbu[*0x801bcb0c] & 3            // saved doorbell low bits
 *   loop @0x80197A2C:
 *     s0 = cd_response_process()          // jal 0x80196370, ra=0x80197A30
 *     if (s0 == 0) break                  // -> exit @0x80197AA4
 *     if (s0 & 4):                        // delay-slot andi @0x80197A3C
 *       cb = *0x801bc84c; if (cb)
 *         jalr cb(a0=lbu[0x801bcb25], a1=0x8020c6d0)  // ra=0x80197A68
 *     if (s0 & 2):                        // recomputed @0x80197A6C (jalr
 *       cb = *0x801bc848; if (cb)         //  clobbered $v0)
 *         jalr cb(a0=lbu[0x801bcb24], a1=0x8020c6c8)  // ra=0x80197A98
 *     goto loop
 *   exit: *(*0x801bcb0c) = s2; return
 *
 * No wait of its own — the only blocking work is inside
 * cd_response_process, whose status waits are CD-hardware/IRQ-updated
 * (same class as cd_cmd_state_reset's proven br2_irq_poll drain loop).
 * The two jalr callbacks are ordinary guest fn ptrs invoked with live
 * a2/a3 leftovers -> cd_jal (faithful jalr: no arg-reg restore, $ra =
 * the guest link token). Guest frame 0x28 reserved so nested calls see
 * the same $sp the MIPS prologue produced. */
/* cd_response_pump — IRQ2-context CD response/completion dispatcher
 * (INTC handler[2] @0x801bcd1c). Faithful clean CPS port lives beside
 * cd_irq_init_seq near the top of this file (0x801979F8 block); it yields/
 * resumes through the CLEAN_ENT_CPS dispatch contract. */

/* ======== 0x80192634 ======== */

/* cd_cmd_queue_load — arm cmd-queue DMA descriptor (cells 0x801B8438-44): end=a0+n*4-4, count=n, trigger 0x11000002, timeout_arm, poll w/ watchdog. */
int cd_cmd_queue_load(uint32_t a0,uint32_t a1)
{
  /* 0x80192634-0x80192710. DMA-descriptor arm for the cmd queue:
   * P1=*cell8444: *P1 |= 0x08000000; P2=*cell8440: *P2 = 0;
   * P3=*cell8438: *P3 = a0 + (a1<<2) - 4  (a0 + a1*4 - 4 —
   * the descriptor END pointer derived from the caller's base arg,
   * NOT from cell8440's pointer value); P4=*cell843C: *P4 = a1;
   * *P2 = 0x11000002; cd_timeout_arm; then while *P2 & 0x01000000:
   * cd_pump_watchdog (ret!=0 -> -1) + irq poll. Returns s0=a1
   * (a1); -1 on pump failure. */
  uint32_t n = a1;
  uint32_t p;
  p = DB32(0x801b8444);
  br2_st32(p, br2_ld32(p) | 0x08000000u);
  p = DB32(0x801b8440);
  br2_st32(p, 0);
  br2_st32(DB32(0x801b8438), a0 + (n << 2) - 4u);
  br2_st32(DB32(0x801b843c), n);
  p = DB32(0x801b8440);
  br2_st32(p, 0x11000002u);
  cd_timeout_arm();
  p = DB32(0x801b8440);
  while ((br2_ld32(p) & 0x01000000u) != 0) {
    if (cd_pump_watchdog() != 0) return -1;
    if (br2_bail_pending()) return -1;
    br2_irq_poll();
    p = DB32(0x801b8440);
  }
  return (int)n;
}

/* ======== 0x801938c8 ======== */

/* cd_gpu_call_20 — rect dbg 0x801255EC, timeout_arm, wait_stat, dispatch tbl4(2,0x80193CE0), gpu_invoke(tbl+0x20). */
int32_t cd_gpu_call_20(uint32_t a0,uint32_t a1)
{
  int32_t r;
  gpu_rect_bounds_dbg(0x801255ec,(short *)(uintptr_t)a0);
  cd_timeout_arm();
  r = cd_wait_stat();
  if (r != 0) return r;
  cd_dispatch_tbl4(2,0x80193ce0);
  {
    uint32_t tbl = DB32G(0x801b8318);
    gpu_invoke(*(uint32_t *)guest_at(tbl + 0x20),a0,a1,0,0);
  }
  return 0;
}

/* ======== 0x801939b4 ======== */

/* cd_gpu_call_1c — same shape, rect dbg 0x80125510, gpu_invoke(tbl+0x1C). */
int32_t cd_gpu_call_1c(uint32_t a0,uint32_t a1)
{
  int32_t r;
  gpu_rect_bounds_dbg(0x80125510,(short *)(uintptr_t)a0);
  cd_timeout_arm();
  r = cd_wait_stat();
  if (r != 0) return r;
  cd_dispatch_tbl4(2,0x80193ce0);
  {
    uint32_t tbl = DB32G(0x801b8318);
    gpu_invoke(*(uint32_t *)guest_at(tbl + 0x1c),a0,a1,0,0);
  }
  return 0;
}

/* 0x8019a814 — cd_chan_full_init. This function is the resumable half of
 * the CD deferred-interrupt contract: it calls cd_ctx_save(0x801bcd50)
 * which snapshots the LIVE guest register file, registers that frame via
 * B0/0x19 -> mem[0x4C18], and on CD IRQ the kernel calls cd_ctx_restore
 * (0x8019adcc) which reloads the frame and re-enters here with v0!=0 to
 * run cd_intc_dispatch. That save/restore operates on cpu->gpr, which a
 * clean C body does not own — the host stack is invisible to it. The whole
 * function therefore runs generated; the thunk keeps direct C callers
 * correct and makes gen_clean_table skip the address. */
uint32_t cd_chan_full_init(void)
{
  return call_guest4(0x8019a814, 0, 0, 0, 0);
}

/* ======== 0x801918a0 ======== */

/* cd_prim_emit_a — prim emit w/ optional trace cb 0x80125584 (>=2); builds packet via gpu tbl. */
void cd_prim_emit_a(uint32_t a0,uint32_t a1)
{
  uint32_t s0;
  uint32_t tbl;
  if (DB8(0x801b8322) >= 2) {
    call_guest(DB32G(0x801b831c),0x80125584,a0);
  }
  s0 = a1 + 0x1c;
  gpu_prim_pkt_build((int)s0,(uint16_t *)(uintptr_t)a1);
  *(uint32_t *)guest_at(a1 + 0x1c) =
      (DB32(a1 + 0x1c) & 0xff000000) | (a0 & 0xffffff);
  tbl = DB32G(0x801b8318);
  gpu_invoke(*(uint32_t *)guest_at(tbl + 8),
                 *(uint32_t *)guest_at(tbl + 0x18),s0,0x40,0);
  guest_memcpy((uint32_t *)(uintptr_t)0x801b8330,(uint32_t *)(uintptr_t)a1,0x5c);
}

/* ======== batch_mass8 ======== */

extern uint32_t cd_queue_drain(void);

/* ======== 0x80193574 ======== */

/* cd_queue_poll — a0==0: arm timeout, drain queue until counters 0x801B8448==4C then wait_stat; else bounded poll w/ watchdog. */
int cd_queue_poll(uint32_t a0)
{
  if (a0 == 0) {
    cd_timeout_arm();
    while (DB32(0x801b8448) != DB32(0x801b844c)) {
      if (br2_bail_pending()) return -1;
      cd_queue_drain();
      if (cd_pump_watchdog() != 0) return -1;
    }
    return cd_wait_stat();
  }
  {
    uint32_t s0 = (DB32(0x801b8448) - DB32(0x801b844c)) & 0x3f;
    uint32_t st, o;
    int busy;
    if (s0 != 0) cd_queue_drain();
    st = br2_ld32(DB32G(0x801b8434));
    o = br2_ld32(DB32G(0x801b8428));
    busy = ((st & 0x1000000) != 0) || ((o & 0x04000000) == 0);
    if (busy) return s0 != 0 ? (int)s0 : 1;
    return (int)s0;
  }
}

/* cd_reg_bit40_ctrl — CD register bit40 control @0x8019d030 (21 blocks,
 * disasm-verified). cell = *0x801DDEB0 (reloaded every poll — the guest
 * re-reads the pointer cell each iteration); status half at cell+0x1AA,
 * bit 0x40.
 *
 *   p1==0 or p1==3: clear bit6 (h&=0xFFBF, store, reload); if still set,
 *     bounded poll (~3840 iters, $v1 counter cap 0xF01) for bit6 to read
 *     CLEAR. Timeout -> printf_forward_guest(0x8018E550)
 *     (0x80125C38, 0x80125C48) then return -1.
 *   p1==1 or p1==3: set bit6 (h|=0x40, store, reload); if readback clear,
 *     bounded poll for bit6 to read SET. Timeout -> printf
 *     (0x80125C38, 0x80125C58) then return -1.
 *   All other p1 (and p1==0 after the clear phase): return p1.
 *
 * The polls are device-deadline waits (the bit is cleared/set by the CD
 * status path), so br2_irq_poll per iteration is the faithful completion
 * contract — same as cd_busy_wait/cd_timer_read. The jal 0x8018E550 is
 * printf_forward_guest (non-BIOS varargs): reproduced with a real 0x18
 * guest frame (saved-ra slot at fr+0x10 like the guest prologue) +
 * call_guest4; a2/a3 are never written by this function so the live
 * caller gpr[6]/gpr[7] are forwarded exactly. */
int cd_reg_bit40_ctrl(uint32_t a0)
{
  uint32_t cell, h, cnt, a1v = 0;

  if (a0 == 0 || a0 == 3) {
    /* @0x8019D048: h = lh(cell+0x1AA) & ~0x40; store; reload; test bit6 */
    cell = br2_ld32(0x801ddeb0);
    h = (uint32_t)br2_ld16(cell + 0x1aau) & 0xffbfu;
    br2_st16(cell + 0x1aau, (uint16_t)h);
    h = (uint32_t)br2_ld16(cell + 0x1aau);
    if ((h & 0x40u) != 0) {
      /* @0x8019D078..0x8019D0B8: cnt=1; while(cnt<0xF01){reload,test,
       * cnt++}; bit still set -> @0x8019D088 timeout printf(…5C48) -1 */
      cnt = 1;
      for (;;) {
        if (cnt >= 3841u) { a1v = 0x80125c48u; goto timeout; }
        cell = br2_ld32(0x801ddeb0);
        cnt++;
        if (((uint32_t)br2_ld16(cell + 0x1aau) & 0x40u) == 0) break;
        if (br2_bail_pending()) return -1;
        br2_irq_poll();
      }
    }
  }
  if (a0 == 1 || a0 == 3) {
    /* @0x8019D0D4: h = lh(cell+0x1AA) | 0x40; store; reload; test bit6 —
     * set already -> return p1 (@0x8019D158); clear -> bounded poll for
     * bit6 to read SET, timeout printf(…5C58) -1 (@0x8019D114). */
    cell = br2_ld32(0x801ddeb0);
    h = (uint32_t)br2_ld16(cell + 0x1aau) | 0x40u;
    br2_st16(cell + 0x1aau, (uint16_t)h);
    h = (uint32_t)br2_ld16(cell + 0x1aau);
    if ((h & 0x40u) == 0) {
      cnt = 1;
      for (;;) {
        if (cnt >= 3841u) { a1v = 0x80125c58u; goto timeout; }
        cell = br2_ld32(0x801ddeb0);
        cnt++;
        if (((uint32_t)br2_ld16(cell + 0x1aau) & 0x40u) != 0) break;
        if (br2_bail_pending()) return -1;
        br2_irq_poll();
      }
    }
    return (int)a0;                        /* @0x8019D158: v0 = a0 */
  }
  return (int)a0;                          /* @0x8019D15C: v0 = a0 */

timeout:                                        /* @0x8019D124: jal 0x8018E550 */
  {
    uint32_t fr = br2_guest_frame(0x18);
    /* guest frame is 0x18 with ra saved at sp+0x10 — the printf vararg
     * wrapper's first stack-arg slot reads that same cell; mirror it. */
    br2_st32(fr + 0x10, clean_cpu_state()->gpr[31]);
    (void)call_guest4(0x8018e550, 0x80125c38u, a1v,
                      clean_cpu_state()->gpr[6], clean_cpu_state()->gpr[7]);
    if (br2_bail_pending()) { br2_guest_frame_abandon(); return -1; }
    br2_guest_frame_free();
    return -1;                                  /* @0x8019D130: v0 = -1 */
  }
}

/* ======== batch_mass9 ======== */

extern void cd_svc_dual(uint32_t);
extern uint32_t cd_queue_b_read(void);
extern int32_t bios_tramp_b035(int32_t,const void *,int32_t);

/* ======== 0x8019f02c ======== */

/* cd_job_sm5 — job SM state 5 (cb 0x8019F02C): init->wait FUN_801A0230/240 pumps->0x1e; on queue status retry<=3 then map status, ret 1 done. */
int cd_job_sm5(uint32_t a0)
{
  uint32_t *s1 = (uint32_t *)guest_at(a0);
  uint32_t v1 = *s1;
  if (v1 == 0xa || v1 == 0) {
    if (v1 == 0) {
      DB32(0x801be74c) = 0;
      *s1 = 0xa;
    }
    /* state 0xa: wait pump counters then arm 0x1e */
    {
      uint32_t *s0 = (uint32_t *)guest_at(0x801febf4);
      uint32_t want;
      for (;;) {
        if (br2_bail_pending()) return 0;
        want = (uint32_t)bios_tramp_b033(s0[0],s0[1],0,0);
        if (want == s0[1]) break;
      }
      cd_pending_args_pump();
      for (;;) {
        if (br2_bail_pending()) return 0;
        if (bios_tramp_b034(s0[0],s0[3],s0[2],0) == 0) break;
      }
      *s1 = 0x1e;
      return 0;
    }
  }
  if (v1 < 0xb) return 0;
  if (v1 == 0x1e) {
    uint32_t a0;
    if (cd_queue_a_read() == 0) return 0;
    a0 = cd_queue_a_status();
    if (a0 != 0) {
      uint32_t cnt = DB32(0x801be74c) + 1;
      DB32(0x801be74c) = cnt;
      if (cnt < 4) {
        *s1 = 0xa;
        return 0;
      }
    } else {
      a0 = 0;
    }
    DB32(0x801febe4) = cd_status_map(a0);
    return 1;
  }
  return 0;
}

/* ======== 0x8019f220 ======== */

/* cd_job_sm6 — job SM state 6 (cb 0x8019F220): same skeleton via bios_tramp_b035 instead of FUN_801A0240. */
int cd_job_sm6(uint32_t a0)
{
  uint32_t *s1 = (uint32_t *)guest_at(a0);
  uint32_t v1 = *s1;
  if (v1 == 0xa || v1 == 0) {
    if (v1 == 0) {
      DB32(0x801be750) = 0;
      *s1 = 0xa;
    }
    {
      uint32_t *s0 = (uint32_t *)guest_at(0x801febf4);
      uint32_t want;
      for (;;) {
        if (br2_bail_pending()) return 0;
        want = (uint32_t)bios_tramp_b033(s0[0],s0[1],0,0);
        if (want == s0[1]) break;
      }
      cd_pending_args_pump();
      for (;;) {
        if (br2_bail_pending()) return 0;
        if (bios_tramp_b035((int32_t)s0[0],guest_at(s0[3]),(int32_t)s0[2]) == 0) break;
      }
      *s1 = 0x1e;
      return 0;
    }
  }
  if (v1 < 0xb) return 0;
  if (v1 == 0x1e) {
    uint32_t a0;
    if (cd_queue_a_read() == 0) return 0;
    a0 = cd_queue_a_status();
    if (a0 != 0) {
      uint32_t cnt = DB32(0x801be750) + 1;
      DB32(0x801be750) = cnt;
      if (cnt < 4) {
        *s1 = 0xa;
        return 0;
      }
    } else {
      a0 = 0;
    }
    DB32(0x801febe4) = cd_status_map(a0);
    return 1;
  }
  return 0;
}

/* ======== 0x8019f474 ======== */

/* cd_job_submit5 — job submit SM for state-5 job (cb 0x8019F02C): case walk 0->alloc, 0xa->FUN_801A0220 slot, 0x14->pump svc_dual, 0x16->queue_b. */
int cd_job_submit5(uint32_t a0)
{
  uint32_t *s1 = (uint32_t *)guest_at(a0);
  switch (*s1) {
  case 0:
    DB32(0x801be754) = 0;
    cd_job_slot_alloc(0x8019e798,0);
    *s1 = 0xa;
    return 0;
  case 10: {
    uint32_t *s0 = (uint32_t *)guest_at(0x801febe4);
    if (s0[0] != 0) return 1;
    {
      int32_t r = (int32_t)bios_tramp_b032(0x801fec04,0x8001,0,0);
      s0[4] = (uint32_t)r;
      if (r < 0) {
        s0[0] = 5;
        return 1;
      }
    }
    *s1 = 0x14;
    cd_job_slot_alloc(0x8019f02c,0);
    return 0;
  }
  case 11:
    *s1 = 0x14;
    cd_job_slot_alloc(0x8019f02c,0);
    return 0;
  case 20: {
    uint32_t *s0 = (uint32_t *)guest_at(0x801febe4);
    if (s0[0] == 3) {
      cd_pending_args_pump();
      cd_svc_dual(s0[3]);
      *s1 = 0x16;
      return 0;
    }
    bios_tramp_b036(s0[4],0,0,0);
    s0[4] = 0xffffffffu;
    return 1;
  }
  case 22:
    if (cd_queue_b_read() == 0) return 0;
    cd_queue_b_status();
    *s1 = 0xb;
    return 0;
  default:
    return 0;
  }
}

/* ======== 0x8019f6cc ======== */

/* cd_job_submit6 — sibling submit SM for state-6 job (cb 0x8019F220). */
int cd_job_submit6(uint32_t a0)
{
  uint32_t *s1 = (uint32_t *)guest_at(a0);
  switch (*s1) {
  case 0:
    DB32(0x801be758) = 0;
    cd_job_slot_alloc(0x8019f220,0);
    *s1 = 0xa;
    return 0;
  case 10: {
    uint32_t *s0 = (uint32_t *)guest_at(0x801febe4);
    if (s0[0] != 0) return 1;
    {
      int32_t r = (int32_t)bios_tramp_b032(0x801fec04,0x8001,0,0);
      s0[4] = (uint32_t)r;
      if (r < 0) {
        s0[0] = 5;
        return 1;
      }
    }
    *s1 = 0x14;
    cd_job_slot_alloc(0x8019f220,0);
    return 0;
  }
  case 11:
    *s1 = 0x14;
    cd_job_slot_alloc(0x8019f220,0);
    return 0;
  case 20: {
    uint32_t *s0 = (uint32_t *)guest_at(0x801febe4);
    if (s0[0] == 3) {
      cd_pending_args_pump();
      cd_svc_dual(s0[3]);
      *s1 = 0x16;
      return 0;
    }
    bios_tramp_b036(s0[4],0,0,0);
    s0[4] = 0xffffffffu;
    return 1;
  }
  case 22:
    if (cd_queue_b_read() == 0) return 0;
    cd_queue_b_status();
    *s1 = 0xb;
    return 0;
  default:
    return 0;
  }
}

/* ======== batch_mass10 ======== */

/* ======== 0x80191ee0 ======== */

/* cd_vcall_38 — indirect vtable call: fn=*(a0+0x38); call_guest(fn,a0,a1); return sign bit (>>31). */
uint32_t cd_vcall_38(uint32_t a0,uint32_t a1)
{
  uint32_t fn = *(uint32_t *)guest_at(a0 + 0x38);
  return call_guest(fn,a0,a1) >> 31;
}

/* ======== 0x8019a688 ======== */

/* cd_vcall_c — vtable slot 0xC: call_guest(*(a0+0xC),a0,a1). */
uint32_t cd_vcall_c(uint32_t a0,uint32_t a1)
{
  uint32_t fn = *(uint32_t *)guest_at(a0 + 0xc);
  return call_guest(fn,a0,a1);
}

/* ======== 0x8019a6b8 ======== */

/* cd_vcall_8 — vtable slot 8. */
uint32_t cd_vcall_8(uint32_t a0,uint32_t a1)
{
  uint32_t fn = *(uint32_t *)guest_at(a0 + 8);
  return call_guest(fn,a0,a1);
}

/* ======== 0x8019a6e8 ======== */

/* cd_vcall_4 — vtable slot 4. */
uint32_t cd_vcall_4(uint32_t a0,uint32_t a1)
{
  uint32_t fn = *(uint32_t *)guest_at(a0 + 4);
  return call_guest(fn,a0,a1);
}

/* ======== 0x8019a74c ======== */

/* cd_vcall_14 — vtable slot 0x14. */
uint32_t cd_vcall_14(uint32_t a0,uint32_t a1)
{
  uint32_t fn = *(uint32_t *)guest_at(a0 + 0x14);
  return call_guest(fn,a0,a1);
}

/* ======== 0x8019a77c ======== */

/* cd_vcall_10 — vtable slot 0x10. */
uint32_t cd_vcall_10(uint32_t a0,uint32_t a1)
{
  uint32_t fn = *(uint32_t *)guest_at(a0 + 0x10);
  return call_guest(fn,a0,a1);
}

/* ======== 0x8019a7ac ======== */

/* cd_vcall_18 — vtable slot 0x18. */
uint32_t cd_vcall_18(uint32_t a0,uint32_t a1)
{
  uint32_t fn = *(uint32_t *)guest_at(a0 + 0x18);
  return call_guest(fn,a0,a1);
}

/* ======== 0x8019799c ======== */

/* cd_mmio_bit_wait — spin until bit6 of byte at a1 sets; irq_poll each pass, bail-aware. */
int cd_mmio_bit_wait(uint32_t a0,uint32_t a1)
{
  volatile uint8_t *p = (volatile uint8_t *)guest_at(a1);
  while ((*p & 0x40) == 0) {
    if (br2_bail_pending()) return 0;
    br2_irq_poll();
  }
  br2_st32(DB32G(0x801bcb50), 0x11400100);
  (void)br2_ld32(DB32G(0x801bcb50));
  return 0;
}

/* ======== 0x80199184 ======== */

/* Alternate entry into cd_file_open_b (0x8019917c) at +8: the flag-cell
 * load at 0x8019917c/0x80199180 is skipped, so the branch at 0x80199190
 * tests the CALLER's v0, not *(0x801fc6f8). The rest of the body is
 * identical: s0=a0 -> cd_file_open(cd_stamp_to_sector(0x801feb00)+1, a0)
 * -> return *(0x801feb04). Guest contract: a0 = incoming v0 (flag),
 * a1 = incoming a0 (path guest addr) — the clean_table wrapper
 * marshals gpr[2]/gpr[4] for this addr (ARG_GPRS in gen_clean_table.py).
 * Disasm 0x80199184-0x801991d0 (inside br2eu_func_8019917C). */
int cd_file_open_seq(uint32_t a0,uint32_t a1)
{
  int32_t v0;
  if (a0 != 0) return -1;
  v0 = cd_stamp_to_sector(guest_at(0x801FEB00));
  cd_seek_note(3, *(uint32_t *)guest_at(0x801FEB00), (uint32_t)v0);
  cd_file_open(v0 + 1,(uint8_t *)guest_at(a1));
  return (int)*(int32_t *)guest_at(0x801feb04);
}

/* ======== 0x80191184 ======== */

/* cd_gpu_call_3c — if a1>=2 call_guest(DB32G(0x801B831C),0x801254B4,a0); then call tbl+0x3C fn(a0,0). */
int cd_gpu_call_3c(uint32_t a0,uint32_t a1)
{
  if (a1 >= 2) {
    uint32_t fn = DB32G(0x801b831c);
    call_guest(fn,0x801254b4,a0);
  }
  {
    uint32_t tbl = DB32G(0x801b8318);
    uint32_t fn = *(uint32_t *)guest_at(tbl + 0x3c);
    return (int)call_guest(fn,a0,0);
  }
}

/* ======== 0x8019a24c ======== */

/* cd_hdr_clear — clear u16 @a0+0xA, dead countdown loop (fidelity). */
void cd_hdr_clear(uint32_t a0)
{
  volatile int i;
  *(uint16_t *)guest_at(a0 + 0xa) = 0;
  for (i = 9; i != -1; i--) {}
}

/* ======== batch_mass11 ======== */

/* ======== 0x80191778 ======== */

/* cd_gpu_call_8b — if a1>=2 call_guest(DB32G(0x801B831C),0x80125558,a0); then call tbl+8 fn(tbl+0x18,a0). */
void cd_gpu_call_8b(uint32_t a0,uint32_t a1)
{
  if (a1 >= 2) {
    uint32_t fn = DB32G(0x801b831c);
    call_guest(fn,0x80125558,a0);
  }
  {
    uint32_t tbl = DB32G(0x801b8318);
    uint32_t f2 = *(uint32_t *)guest_at(tbl + 8);
    uint32_t a0v = *(uint32_t *)guest_at(tbl + 0x18);
    call_guest(f2,a0v,a0);   /* a2/a3 zero in guest */
  }
}

/* ======== 0x8019b310 ======== */

/* cd_dma_chan_init — delegate whole body to generated MIPS (0x8019b310).
 * Same BIOS `jr 0xB0` OpenEvent/EnableEvent boundary as cd_ext_init_once —
 * the $v0 handle must propagate through generated context (session-18). */
void cd_dma_chan_init(uint32_t a0)
{
  (void)a0;
  call_guest4(0x8019b310u,a0, 0, 0, 0);
}

/* ======== 0x80199100 ======== */

/* cd_dir_entry_store — store dir entry (stride 0x20 by a0): tag=2, unaligned u32->0x801FEB00, extent->0x801FEB04, feb00_note, fire cb @0x801FEB10, clear 0x801FC69C. */
void cd_dir_entry_store(uint32_t a0,uint32_t a1)
{
  uint8_t *e = (uint8_t *)guest_at(a1 + ((uint32_t)a0 << 5));
  uint32_t t;
  uint32_t fn;
  *(uint16_t *)e = 2;
  memcpy(&t,e + 0x1c,4);                     /* lwl/lwr unaligned u32 */
  *(uint32_t *)guest_at(0x801feb00) = t;
  *(uint32_t *)guest_at(0x801feb04) = *(uint32_t *)(e + 8);
  feb00_note(2, t);
  DB32(0x801fc6a8) = DB32(0x801fc6a4);
  fn = DB32G(0x801feb10);
  if (fn != 0) call_guest(fn,fn,0);        /* jalr $a0: a0 = fn ptr */
  DB32(0x801fc69c) = 0;
}

/* ======== 0x8019d508 ======== */

extern int32_t cd_seek_state_machine(uint32_t);

/* cd_seek_step — trace; delegate to generated 0x8019D508 (TestEvent IRQ-poll spin needs deferred CD coroutine). */
int32_t cd_seek_step(uint32_t a0,uint32_t a1)
{
  CDFN(CDFN_SEEK_STEP,a0);
  /* IRQ-poll spin on TestEvent(0x801bde38) — DeliverEvent needs the deferred
   * CD coroutine resumed on IRQ, which a clean-C spin cannot drive. Delegate
   * to generated (same contract as cd_seek_complete_poll 0x8019d500). */
  return (int32_t)call_guest4(0x8019d508u,a0,a1,0,0);
}

/* ======== 0x8019b8a4 ======== */

extern void cd_mmio_strobe_22(void);

/* cd_dma_xfer_start_b — mid-entry (0x8019B8A4) of cd_dma_xfer_start: MMIO channel setup, args p_a0=buf p_a1=ch p_a2=len; arms DMA cb cell @0x801BDF00. */
void cd_dma_xfer_start_b(uint32_t p_v0,uint32_t p_a0,uint32_t p_a1,uint32_t p_a2)
{
  uint32_t b;
  /* p_v0/b are the MMIO channel block — CDMMIO not guest_at. */
  CDMMIO16_ST(p_v0,0xd3,(uint16_t)p_a1);
  cd_delay_3c();
  b = DB32G(0x801bdeb0);
  CDMMIO16_ST(b,0xd5,(uint16_t)(CDMMIO16(b,0xd5) | 0x30));
  cd_delay_3c();
  p_a2 <<= 16;
  cd_mmio_strobe_22();
  br2_st32(DB32G(0x801bdeb4), p_a0);
  p_a2 |= 0x10;
  br2_st32(DB32G(0x801bdeb8), p_a2);
  DB32(0x801bdf00) = 1;
  br2_st32(DB32G(0x801bdebc), 0x1000200);
}

/* ======== 0x80191670 ======== */

/* cd_gpu_call_2c — if a2>=2 call_guest(DB32G(0x801B831C),0x80125540,a0); then call tbl fn(a0,a1). */
void cd_gpu_call_2c(uint32_t a0,uint32_t a1,uint32_t a2)
{
  if (a2 >= 2) {
    uint32_t fn = DB32G(0x801b831c);
    call_guest(fn,0x80125540,a0);   /* a2 = a1 in guest */
  }
  {
    uint32_t tbl = DB32G(0x801b8318);
    uint32_t f2 = *(uint32_t *)guest_at(tbl + 0x2c);
    call_guest(f2,a0,a1);
  }
  DB32(0x801b83e0) = (0x801b83cc & 0x00ffffff) | 0x400000;
  *(uint32_t *)guest_at(a0) = 0x801b83e0 & 0x00ffffff;
}

/* ======== 0x8019b7e8 ======== */

/* cd_dma_chan_reset — ch reset: settle delay_3c if BDF00==0, clear status bits 0x30 with bounded poll, then DeliverEvent(0xF0000009,0x20) or direct cb @0x801BDEE8. */
void cd_dma_chan_reset(uint32_t a0)
{
  int i;
  /* Generated 0x8019B7E0: the delay_3c gate is mem32[0x801BDF00] == 0
   * (lw $v0,-0x2100($v0) @B7E4; bnez $v0 skips the jal @B7EC), NOT the
   * a0 argument — a0 is never read by the guest body. BDF00 is the DMA
   * callback-arm cell written by cd_dma_xfer_start_b (1) / cleared at
   * 0x8019B9E8 (0); when a ch4 callback is armed the reset skips the
   * ~3-scanline settle delay. */
  (void)a0;
  if (DB32G(0x801bdf00) == 0) cd_delay_3c();
  {
    /* w[0xd5] is the CD channel status register at blk+0x1aa — MMIO via the
     * 0x801bdeb0 pointer cell, must use CDMMIO16 not guest_at (open-bus). */
    uint32_t b = DB32G(0x801bdeb0);
    CDMMIO16_ST(b,0xd5,(uint16_t)(CDMMIO16(b,0xd5) & 0xffcf));
    if ((CDMMIO16(b,0xd5) & 0x30) != 0) {
      for (i = 0; i < 0xf01; i++) {
        b = DB32G(0x801bdeb0);
        if ((CDMMIO16(b,0xd5) & 0x30) == 0) break;
      }
    }
  }
  {
    static int cr_dbg = -1;
    if (cr_dbg < 0) { const char *e = getenv("PSX_CB7"); cr_dbg = (e && e[0] && e[0] != '0') ? 1 : 0; }
    if (cr_dbg) fprintf(stderr, "[chanreset] bdf00=0x%08X dee8=0x%08X -> %s\n",
                        DB32G(0x801bdf00), DB32G(0x801bdee8),
                        DB32G(0x801bdee8) ? "callback" : "DeliverEvent(0xF0000009,0x20)");
  }
  if (DB32G(0x801bdee8) != 0) {
    call_guest(DB32G(0x801bdee8),0xf0000000,0);
  } else {
    (void)cd_bios_svc_07(0xf0000009,0x20);
  }
}

/* ======== 0x801915a8 ======== */

/* cd_prim_list_b — prim-list link walk (a1 count, a2>=2 notify); tail-link @0x801B83E0 same as cd_prim_list_a. */
void cd_prim_list_b(uint32_t a0,uint32_t a1,uint32_t a2)
{
  uint32_t s0 = a0;
  int32_t s1;
  if (a2 >= 2) {
    uint32_t fn = DB32G(0x801b831c);
    call_guest(fn,0x80125528,a0);   /* a2 = a1 in guest */
  }
  s1 = (int32_t)a1 - 1;
  while (s1 != 0) {
    uint32_t a0 = s0 + 4;
    s1--;
    *(uint8_t *)guest_at(s0 + 3) = 0;
    *(uint32_t *)guest_at(s0) =
        (*(uint32_t *)guest_at(s0) & 0xff000000) | a0;
    s0 = a0;
    if (s1 == 0) break;
  }
  DB32(0x801b83e0) = (0x801b83cc & 0x00ffffff) | 0x400000;
  *(uint32_t *)guest_at(s0) = 0x801b83e0 & 0x00ffffff;
}

/* ======== batch_mass12 ======== */

/* ======== 0x8019aabc ======== */

/* cd_chan_slot_set — per-channel slot table update @0x8019AABC (20 blocks,
 * disasm-verified). Table base 0x801C0000-0x32E4 = 0x801BCD1C, 4-int8_t slots
 * indexed by a0; returns the PREVIOUS slot word (v0 = old tbl[a0]).
 *
 * Flow: slot=base+p1*4; old=*slot; if a1==old return old; if
 * lh(0x801BCD18)==0 return old; cell=*(0x801DDDA8); tmp=lh(*cell); *cell=0
 * (half); bits=gpr19 accumulates the channel bit 1<<p1 into/out of the
 * halfword @0x801BCD48 (base+44) — set when a1!=0 (slot<-a1),
 * cleared when a1==0 (slot<-0). Then BIOS-trampoline jals:
 *   p1==0: B0/0x5b tramp(0x80199FE0)(aux, a1=base, a2=base-4, a3=cur)
 *          then C0/0x0a tramp(0x8019A670)(3, aux, ...)
 *   p1==4: C0/0x0a(0, aux); p1==5: (1, aux); p1==6: (2, aux)
 *   aux = (a1==0) ? 1 : 0  (guest sltiu p2<1)
 * a1/a2/a3 stay LIVE guest regs at every jal — values that depend on the
 * guest's full register state at the jal boundary, which clean C cannot
 * reproduce faithfully (a clean port that forwarded guessed constants
 * regressed boot to a gs=0 cd_cmd_issue wedge — verified by bisect).
 * The body sequences BIOS kernel-object calls (B0/0x5b, C0/0x0a) inside a
 * guest CPS frame — the exact UNSAFE PORT PATTERN in AGENTS.md §4. Keep
 * delegated to generated MIPS. */
uint32_t cd_chan_slot_set(uint32_t a0,uint32_t a1)
{
  return (uint32_t)call_guest4(0x8019aabcu,a0,a1,0,0);
}

/* ======== batch_mass13 ======== */

/* ======== 0x8019af84 ======== */

/* cd_cb7_run — DICR done-mask walk: for each set ch bit clear DICR bit, call handler tbl @0x801BDDF4 cursor; tail DMA-error dump on 0x80000000/bit15. */
void cd_cb7_run(uint32_t a0)
{
  uint32_t s3 = 0x00ffffff;
  /* Channel mask comes from DICR via the fixed pointer cell @0x801BDDF0
   * (-> 0x1F8010F4), NOT from a0. The generated body (0x8019AF7C) loads
   * *(0x801BDDF0) then dereferences it; a0 is an INTC-dispatch leftover and
   * is never read. a0 kept for signature parity only. */
  (void)a0;
  uint32_t s1 = (br2_ld32(DB32G(0x801bddf0)) >> 24) & 0x7f;
  {
    static int cb7_dbg = -1;
    if (cb7_dbg < 0) { const char *e = getenv("PSX_CB7"); cb7_dbg = (e && e[0] && e[0] != '0') ? 1 : 0; }
    if (cb7_dbg) fprintf(stderr, "[cb7] donemask=0x%02X dicr=0x%08X\n", s1, br2_ld32(DB32G(0x801bddf0)));
  }
  int s0;
  uint32_t dma, dv;
  uint32_t *tbl;
  while (s1 != 0) {
    dma = DB32G(0x801bddf0);
    tbl = (uint32_t *)guest_at(0x801bddf4);
    s0 = 0;
    while (s1 != 0 && s0 < 7) {
      if ((s1 & 1) != 0) {
        br2_st32(dma, br2_ld32(dma) & ((1u << (s0 + 0x18)) | s3));
        /* tbl is a moving cursor (generated gpr[18] += 4, reads *cursor):
         * dereference *tbl, NOT tbl[s0] — the latter double-indexes and
         * reads the wrong slot. Table base is 0x801BDDF4 (generated cursor
         * = 0x801C0000-8716), NOT 0x801BDDFC: with the wrong base ch4's
         * done bit reads slot 4 at 0x801BDE0C (=0) instead of 0x801BDE04
         * (=0x8019B7E0 cd_dma_chan_reset), so DeliverEvent never fires. */
        uint32_t h = *tbl;
        {
          static int cb7_dbg2 = -1;
          if (cb7_dbg2 < 0) { const char *e = getenv("PSX_CB7"); cb7_dbg2 = (e && e[0] && e[0] != '0') ? 1 : 0; }
          if (cb7_dbg2 && h != 0) fprintf(stderr, "[cb7] ch%d handler=0x%08X\n", s0, h);
        }
        if (h != 0) call_guest(h,0,0);
      }
      tbl++;
      s1 >>= 1;
      s0++;
    }
    s1 = (br2_ld32(dma) >> 24) & 0x7f;
  }
  dma = DB32G(0x801bddf0);
  dv = br2_ld32(dma);
  {
    uint32_t v0 = dv & 0xff000000;
    if (v0 == 0x80000000 || (dv & 0x8000) != 0) {
      uint32_t dma_base = DB32G(0x801bde14);
      printf_forward((const char *)guest_at(0x80125bb8),dv);
      for (s0 = 0; s0 < 7; s0++) {
        printf_forward((const char *)guest_at(0x80125bd4),
                     (uint32_t)s0,br2_ld32(dma_base + s0 * 16));
      }
    }
  }
}

/* ======== 0x8019eda8 ======== */

/* cd_file_open_wait — open-file job loop: FUN_801A0220 alloc slot, job_slot_alloc(0x8019E700), pump until job_result st==0 (retry st==2 x5, st==3 continue). */
int cd_file_open_wait(uint32_t a0,uint32_t a1,uint32_t a2)
{
  uint32_t *q = (uint32_t *)guest_at(0x801febf4);
  uint32_t *s0 = (uint32_t *)guest_at(0x801fec04);
  uint32_t *s4 = (uint32_t *)guest_at(0x801febe0);
  uint32_t st = 0;
  int s2 = 0;
  int32_t v0;
  if ((int32_t)*q >= 0) {
    printf_forward((const char *)guest_at(0x80125de4),0);
    return -1;
  }
  cd_file_header_copy(a0,s0);
  cd_strcat_guard(s0,a1);
  *(uint32_t *)guest_at(0x801febf0) = a0;
  for (;;) {
    if (br2_bail_pending()) return -1;
    v0 = bios_tramp_b032(guest_of(s0),1,0,0);
    if (v0 >= 0) {
      bios_tramp_b036((uint32_t)v0,0,0,0);
      cd_pending_args_pump();
      *q = (uint32_t)(int32_t)bios_tramp_b032(guest_of(s0),a2 | 0x8000,0,0);
      return 0;
    }
    cd_xchg_fec24(0);
    DB32(0x801fec38) = (uint32_t)v0;
    if ((int32_t)s4[0] > 0) {
      printf_forward((const char *)guest_at(0x80125ce8),0);
    } else {
      cd_job_slot_alloc(0x8019e700,0);
      s4[0] = 2;
      s4[1] = 0;
      s4[2] = 0;
      *(uint32_t *)guest_at(0x801febf0) = a0;
    }
    cd_job_result(0,0,&st);
    cd_xchg_fec24(DB32G(0x801fec38));
    if (st == 3) continue;
    if (st == 2) {
      s2++;
      if (s2 < 5) continue;
    }
    if (st != 0) return (int)st;
    return 5;
  }
}


/* ======== batch_mass14 ======== */

/* ======== 0x801936e4 ======== */

/* cd_pump_watchdog — GPU/DMA stall watchdog: on deadline expiry dump regs, reset channel regs via pointer cells, return -1. */
int cd_pump_watchdog(void)
{
  uint32_t v0 = cd_timer_read(-1);
  uint32_t a0;
  if (DB32(0x801b845c) < v0 || ({ a0 = DB32(0x801b8460); DB32(0x801b8460) = a0 + 1; 0xf < a0; })) {
    /* 0x801b8428/34/2c/44 are pointer cells into the 0x1F8010xx/1F8018xx
     * DMA/GPU regs — MMIO, route through br2_ld/st not guest_at. */
    uint32_t a2a = DB32G(0x801b8428);
    uint32_t a3a = DB32G(0x801b8434);
    uint32_t cpa = DB32G(0x801b842c);
    uint32_t diff = (DB32(0x801b8448) - DB32(0x801b844c)) & 0x3f;
    uint16_t r;
    printf_forward((const char *)guest_at(0x801255b8),diff,
                   br2_ld32(a2a),br2_ld32(a3a),br2_ld32(cpa));
    r = cd_u16_swap_store(0);
    DB32(0x801b844c) = 0;
    DB32(0x801b8458) = r;
    DB32(0x801b8448) = 0;
    br2_st32(a3a, 0x401);
    {
      uint32_t p = DB32G(0x801b8444);
      br2_st32(p, br2_ld32(p) | 0x800);
    }
    br2_st32(a2a, 0x200);
    br2_st32(a2a, 0x100);
    cd_u16_swap_store(DB32(0x801b8458));
    return -1;
  }
  return 0;
}

/* ======== 0x80193aa0 ======== */

/* cd_gpu_call_18_b — timed variant of cd_gpu_call_18: arm deadline @0x801B845C=timer+0xF0, validate rect, emit gpu packet via tbl+0x18. */
int cd_gpu_call_18_b(uint32_t a0,uint32_t a1,uint32_t a2)
{
  uint8_t *s0 = (uint8_t *)guest_at(a0);
  uint32_t *a0p;
  uint32_t fn;
  gpu_rect_bounds_dbg(0x8012551c,(short *)s0);
  DB32(0x801b845c) = cd_timer_read(-1) + 0xf0;
  DB32(0x801b8460) = 0;
  if (cd_wait_stat() != 0) return -1;
  cd_dispatch_tbl4(2,0x80193ce0);
  if (*(int16_t *)(s0 + 4) == 0) return -1;
  if (*(int16_t *)(s0 + 6) == 0) return -1;
  a0p = (uint32_t *)guest_at(0x801b83c0);
  a0p[1] = ((uint32_t)a2 << 16) | (a1 & 0xffff);
  a0p[0] = *(uint32_t *)s0;
  a0p[2] = *(uint32_t *)(s0 + 4);
  fn = br2_ld32(DB32G(0x801b8318) + 0x18);
  call_guest(fn,0x801b83b8,0);
  return 0;
}

/* ======== batch_mass15 ======== */

/* ======== 0x8019ae70 ======== */

/* cd_cb8_run_param — VBlank-ish dispatch: bump counter @0x801BDDE0=a0+1, call each of 8 tbl @0x801BDDC0 handlers with (a0,0). */
void cd_cb8_run_param(uint32_t a0)
{
  uint32_t *tbl = (uint32_t *)guest_at(0x801bddc0);
  int i;
  DB32(0x801bdde0) = a0 + 1;
  for (i = 0; i < 8; i++) {
    uint32_t f = tbl[i];
    if (f != 0) call_guest(f,a0,0);
  }
}

/* ======== batch_mass16: disasm-verified ports ======== */

/* mdec_status_read — MDEC status read: cell 0x801b68f4 holds a guest ptr
 * (0x1f801824) to the status register; return *that. 4 insns.
 * Disasm 0x8018fe44-0x8018fe58. */
uint32_t mdec_status_read(void)
{
  uint32_t reg = CDREG32(0x801b68f4);
  if (reg >= 0x1f801000u && reg <= 0x1f803fffu)
    return psx_read_word(reg);
  return CDREG32(reg);
}

/* cd_strncmp — strncmp: a0 = guest string addr, a1 = host buffer
 * (callers pass stack bufs through u32, cf. cd_strncmp12_eq), a2 = max
 * bytes. NULL guards -> 0/-1/1; signed-int8_t diff on mismatch; the
 * count bound is a2-1 (compares at most a2 bytes). Disasm
 * 0x80198510-0x8019858c. */
uint32_t cd_strncmp(uint32_t a0,uint32_t a1,uint32_t a2)
{
  const int8_t *a;
  const int8_t *b;
  int32_t n;
  int8_t ca, cb;
  /* guest tests the raw args; guest_at(0) is non-NULL, so check
   * a0/a1 (a1 arrives as a u32-held host ptr) */
  if (a0 == 0) {
    if (a1 == 0) return 0;
    return (uint32_t)-1;
  }
  if (a1 == 0) return 1;
  a = (const int8_t *)guest_at(a0);
  /* a1 arrives as a raw guest address (clean_table) or a u32-held host
   * pointer — resolve either form via cd_arg_ptr, not a raw cast. */
  b = (const int8_t *)cd_arg_ptr(a1);
  n = (int32_t)a2 - 1;
  if (n < 0) return 0;
  for (;;) {
    ca = *a;
    cb = *b;
    b++;
    if (ca != cb) {
      if (n < 0) return 0;
      return (uint32_t)(int32_t)((int32_t)ca - (int32_t)cb);
    }
    if (ca == 0) return 0;
    a++;
    n--;
    if (n < 0) return 0;
  }
}

/* cd_guest_zero32_a / cd_guest_zero32_b / cd_guest_zero32_c — identical 9-insn
 * helpers: zero a1 u32 words at guest a0 (a1<=0 -> no-op).
 * Disasm 0x8019ad1c, 0x8019af00, 0x8019b1a8. */
void cd_guest_zero32_a(uint32_t a0,uint32_t a1)
{
  uint32_t *p = (uint32_t *)guest_at(a0);
  uint32_t i;
  for (i = 0; i < a1; i++) p[i] = 0;
}

/* cd_guest_zero32_b — zero a1 u32 words at guest a0 (a1<=0 -> no-op); guest 0x8019af00. */
void cd_guest_zero32_b(uint32_t a0,uint32_t a1)
{
  uint32_t *p = (uint32_t *)guest_at(a0);
  uint32_t i;
  for (i = 0; i < a1; i++) p[i] = 0;
}

/* cd_guest_zero32_c — zero a1 u32 words at guest a0 (a1<=0 -> no-op); guest 0x8019b1a8. */
void cd_guest_zero32_c(uint32_t a0,uint32_t a1)
{
  uint32_t *p = (uint32_t *)guest_at(a0);
  uint32_t i;
  for (i = 0; i < a1; i++) p[i] = 0;
}

/* cd_state_store — store a0 into 0x801bdee8 only when different.
 * Disasm 0x8019d5c0-0x8019d5e0. */
void cd_state_store(uint32_t a0)
{
  if (CDREG32(0x801bdee8) != a0) CDREG32(0x801bdee8) = a0;
}

/* cd_queue_b_read — queue-B packed word (no side effects):
 * qa0 + qa4*2 + qa8*4 + qac*8 @0x801fefa0-ac. Same shape as the head
 * of cd_queue_b_status. Disasm 0x801a1184-0x801a11bc. */
uint32_t cd_queue_b_read(void)
{
  return CDREG32(0x801fefa0)
       + (CDREG32(0x801fefa4) << 1)
       + (CDREG32(0x801fefa8) << 2)
       + (CDREG32(0x801fefac) << 3);
}

/* ======== batch_mass17: disasm-verified ports ======== */

/* cd_queue_drain — CD command-queue drain (0x801931c4-0x80193420).
 * Ring of 64 entries @0x801fae70, stride 96: +0 fn, +4 data, +8 arg;
 * head=0x801b8448, tail=0x801b844c (mod 64). Early-outs: hw status
 * bit 0x01000000 set -> 1; empty queue -> epilogue only. Per entry:
 * reschedule via cd_dispatch_tbl4(2,0) when one slot remains and
 * 0x801b832c==0, busy-wait *0x801b8428 & 0x400000, dispatch
 * fn(data,arg), advance tail; loop while queue non-empty and status
 * bit clear. Epilogue restores cd_u16_swap_store save @0x801b8454, fires
 * pending cb @0x801b832c once (clearing flag @0x801b8328), returns
 * 1 on the entry status early-out else (head-tail)&0x3f. */
uint32_t cd_queue_drain(void)
{
  /* Re-verified against generated 0x801931C4-0x80193420: status mask is
   * 0x01000000 (lui 0x0100), not 0x100000 — earlier clean body had the
   * wrong bit. Busy-waits now bail-poll like every other clean-C spin. */
  if ((br2_ld32(DB32G(0x801b8434)) & 0x01000000u) != 0)
    return 1;
  DB32(0x801b8454) = cd_u16_swap_store(0);
  if (DB32(0x801b8448) != DB32(0x801b844c)
      && (br2_ld32(DB32G(0x801b8434)) & 0x01000000u) == 0) {
    for (;;) {
      uint32_t tail = DB32(0x801b844c);
      uint32_t *e;
      uint32_t fn, dp, arg;
      if (((tail + 1) & 0x3f) == DB32(0x801b8448) && DB32(0x801b832c) == 0)
        cd_dispatch_tbl4(2,0);
      while ((br2_ld32(DB32G(0x801b8428)) & 0x04000000u) == 0) {
        if (br2_bail_pending()) return 0;
        br2_irq_poll();
      }
      e = (uint32_t *)guest_at(0x801fae70 + tail * 96);
      fn = e[0]; dp = e[1]; arg = e[2];
      call_guest(fn,dp,arg);          /* jalr fn: a0=data a1=arg */
      DB32(0x801b844c) = (tail + 1) & 0x3f;
      if (DB32(0x801b8448) == DB32(0x801b844c)) break;
      if ((br2_ld32(DB32G(0x801b8434)) & 0x01000000u) != 0) break;
    }
  }
  cd_u16_swap_store((uint16_t)DB32(0x801b8454));
  if (DB32(0x801b8448) == DB32(0x801b844c)
      && (br2_ld32(DB32G(0x801b8434)) & 0x01000000u) == 0
      && DB32(0x801b8328) != 0) {
    uint32_t cb = DB32(0x801b832c);
    if (cb != 0) {
      DB32(0x801b8328) = 0;
      call_guest(cb,cb,0);            /* jalr a0: a0 = cb ptr */
    }
  }
  return (DB32(0x801b8448) - DB32(0x801b844c)) & 0x3f;
}

/* cd_queue_push — CD command-queue push (0x80192f14-0x801931c0).
 * a0 = fn ptr, a1 = data ptr (guest addr), a2 = payload int8_t count
 * (copied as a2/4 u32 words into entry+0xc), a3 = arg. Preamble
 * cd_timeout_arm; full queue ((head+1)&0x3f==tail) -> cd_pump_watchdog
 * pump + cd_queue_drain drain, -1 on pump failure. Saves hw reg via
 * cd_u16_swap_store(0) -> 0x801b8450, sets flag @0x801b8328. Direct-call
 * fast path when int8_t @0x801b8321==0, or queue empty + status bit
 * 0x01000000 clear + @0x801b832c==0: busy-wait *0x801b8428 & 0x400000,
 * fn(data,arg), restore reg, return 0. Otherwise enqueue entry,
 * head=(head+1)&0x3f, restore reg, cd_queue_drain(), return depth. */
int cd_queue_push(uint32_t a0_fn,uint32_t a1_data,uint32_t a2_count,
                 uint32_t a3_arg)
{
  /* Re-verified against generated 0x80192F14-0x801931C0: status mask is
   * 0x01000000 (lui 0x0100), not 0x100000. Busy-waits bail-poll. */
  uint32_t head, tail, saved;
  int direct;
  cd_timeout_arm();
  for (;;) {
    head = DB32(0x801b8448);
    tail = DB32(0x801b844c);
    if (((head + 1) & 0x3f) != tail) break;  /* guest: addiu+andi 0x3f+beq */
    if (cd_pump_watchdog() != 0) return -1;
    cd_queue_drain();
    if (br2_bail_pending()) return -1;
    br2_irq_poll();
  }
  saved = cd_u16_swap_store(0);
  DB32(0x801b8450) = saved;
  DB32(0x801b8328) = 1;
  direct = (DB8(0x801b8321) == 0);
  if (!direct) {
    head = DB32(0x801b8448);
    tail = DB32(0x801b844c);
    direct = (head == tail
              && (br2_ld32(DB32G(0x801b8434)) & 0x01000000u) == 0
              && DB32(0x801b832c) == 0);
  }
  if (direct) {
    while ((br2_ld32(DB32G(0x801b8428)) & 0x04000000u) == 0) {
      if (br2_bail_pending()) return -1;
      br2_irq_poll();
    }
    call_guest(a0_fn,a1_data,a3_arg);  /* jalr s3: a0=s0 a1=s2 */
    cd_u16_swap_store((uint16_t)DB32(0x801b8450));
    return 0;
  }
  cd_dispatch_tbl4(2,0x801931c4);
  head = DB32(0x801b8448);
  {
    uint32_t *e = (uint32_t *)guest_at(0x801fae70 + head * 96);
    if (a2_count != 0) {
      int32_t n = (int32_t)a2_count / 4;  /* sra-adjusted: trunc to 0 */
      uint32_t *src = (uint32_t *)guest_at(a1_data);
      int32_t i;
      for (i = 0; i < n; i++) e[3 + i] = src[i];
      e[1] = 0x801fae7c + head * 96;      /* payload ptr (guest addr) */
    } else {
      e[1] = a1_data;
    }
    e[2] = a3_arg;
    e[0] = a0_fn;
  }
  DB32(0x801b8448) = (head + 1) & 0x3f;
  cd_u16_swap_store((uint16_t)DB32(0x801b8450));
  cd_queue_drain();
  return (int)((DB32(0x801b8448) - DB32(0x801b844c)) & 0x3f);
}

/* cd_ctx_save — guest context save (0x8019ad90-0x8019adc8): stores
 * ra,sp,fp,s0-s7,gp into the 12-word frame at a0, returns 0. Paired
 * restore is 0x8019adcc (returns a1 via register reload). This is the
 * deferred-interrupt save half: it must capture the live GUEST register
 * file so cd_ctx_restore can later resume cd_chan_full_init mid-body.
 * The register file IS live in cpu->gpr at the clean-call boundary —
 * clean_try_dispatch/call_guest preserve s0-s7/gp/sp/fp/ra, so reading
 * clean_cpu_state()->gpr[] reproduces the guest sw sequence exactly. */
int cd_ctx_save(uint32_t a0)
{
  CPUState *c = clean_cpu_state();
  br2_st32(a0 +  0, c->gpr[31]);  /* sw $ra,  0(a0)  */
  br2_st32(a0 + 44, c->gpr[28]);  /* sw $gp, 44(a0)  */
  br2_st32(a0 +  4, c->gpr[29]);  /* sw $sp,  4(a0)  */
  br2_st32(a0 +  8, c->gpr[30]);  /* sw $fp,  8(a0)  */
  br2_st32(a0 + 12, c->gpr[16]);  /* sw $s0, 12(a0)  */
  br2_st32(a0 + 16, c->gpr[17]);  /* sw $s1, 16(a0)  */
  br2_st32(a0 + 20, c->gpr[18]);  /* sw $s2, 20(a0)  */
  br2_st32(a0 + 24, c->gpr[19]);  /* sw $s3, 24(a0)  */
  br2_st32(a0 + 28, c->gpr[20]);  /* sw $s4, 28(a0)  */
  br2_st32(a0 + 32, c->gpr[21]);  /* sw $s5, 32(a0)  */
  br2_st32(a0 + 36, c->gpr[22]);  /* sw $s6, 36(a0)  */
  br2_st32(a0 + 40, c->gpr[23]);  /* sw $s7, 40(a0)  */
  return 0;                       /* move $v0,$zero ; jr $ra */
}

/* cd_intc_dispatch — CD interrupt dispatch (0x8019a8ec-0x8019aab8).
 * State block @0x801bcd18: +0 u16 in-service flag, +2 u16 busy flag,
 * +4 u16 mask C, +4.. handler table u32[11] @0x801bcd1c. Mask =
 * *ptrA(0x801bdda4) & *ptrB(0x801bdda8) & u16@0x801bcd48; per set bit
 * i<11: *ptrA = ~(1<<i), call tbl[i] (args indeterminate in guest —
 * stale regs — so 0,0). Re-polls until mask drains. Tail: while
 * *ptrA&*ptrB nonzero bump counter @0x801bddb0; at old>=0x801 print
 * "intr timeout(%04x:%04x)", reset counter, clear *ptrA. Mask==0
 * also clears counter. Ends with BIOS svc 0x17 (FUN_8019ad60
 * trampoline: intc enable) and busy flag=0. First entry with
 * flag==0 prints "unexpected interrupt(%04x)" then enables intc.
 * Disasm 0x8019a8ec-0x8019aab8. */
/* cd_intc_dispatch — IRQ bitmask dispatch (INTC cause -> 11 handlers).
 * DELEGATED to generated (0x8019a8ec): part of the 16c4e96 CD-port batch
 * that wedged attract/boot (~frame 15k, pending-cmd starvation). The
 * IRQ-dispatch semantics need the generated CPS per-block interrupt /
 * live-resume-PC contract; stays generated-authoritative per §4. */
void cd_intc_dispatch(void)
{
  call_guest4(0x8019a8ecu,0,0,0,0);
}
/* bios_tramp_b035 — BIOS trampoline only (0x8018ef50-0x8018ef58):
 * addiu t2,0xb0; jr t2; t1=0x35. Kernel svc 0x35 (line-buffer
 * flush used by the printf emit layer). a2 is dropped at the
 * bios_svc boundary (svc/a1 unused by bios_call). */
int32_t bios_tramp_b035(int32_t a0,const void *a1,int32_t a2)
{
  /* callers pass a host pointer into guest RAM; the BIOS service expects a
   * guest address, so translate back through guest_of. svc 0x35 forwards
   * a0-a2 (line/buf/count). */
  return (int32_t)bios_svc6(0xb0,0x35,(uint32_t)a0,
                              guest_of(a1),(uint32_t)a2,0);
}

/* cd_hw_chan_init — channel/hw object init under DI/EI (0x801a0be0-
 * 0x801a0db8). Eight FUN_8019b380(object,attr,size,cb) allocations —
 * objects 0xf4000001 / 0xf0000011, attrs {4,0x8000,0x100,0x2000},
 * size 0x1000, guest cb addrs 0x801a0b10..0x801a0b9c — stored to
 * 0x8020ef70..0x8020ef8c, then each released via FUN_8019b390 in
 * order, cd_pending_args_pump() pump, conditional EI. Guest tests the DI
 * (syscall 1) return ==1 before EI; host FUN_8018e300 is a void
 * stub, so the restore runs unconditionally — same convention as
 * every other DI/EI site in this file. */
/* cd_hw_chan_init — delegate whole body to generated MIPS (0x801a0be0).
 * Eight FUN_8019b380(B0/0x08) allocs + FUN_8019b390(B0/0x09) frees: the
 * BIOS `jr 0xB0` $v0 handles must propagate through generated context
 * (session-18 boundary bug — same as cd_ext_init_once/cd_dma_chan_init). */
void cd_hw_chan_init(void)
{
  call_guest4(0x801a0be0u,0, 0, 0, 0);
}

/* ======== 0x8019ad60 ======== */

/* bios_tramp_b017 — BIOS B0/0x17 trampoline (INTC enable; tail of
 * cd_intc_dispatch). Guest: `addiu t2,zero,0xb0; jr t2; addiu t1,zero,
 * 0x17` — forwards caller a0-a3 untouched, returns the service $v0.
 * Disasm 0x8019ad60-0x8019ad6c. */
uint32_t bios_tramp_b017(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3)
{
  return bios_svc6(0xb0,0x17,a0,a1,a2,a3);
}

/* ======== 0x801a0af8 ======== */

/* job_counter_sign — sign-bit read of the job-slot counter @0x801be768
 * (the same cell cd_job_slot_alloc/cd_job_slots_reset maintain).
 * Guest: `lui v0,0x801c; lw v0,-0x1898(v0); jr ra; srl v0,v0,0x1f` —
 * returns 1 when the counter word is negative, else 0. Disasm
 * 0x801a0af8-0x801a0b04. */
uint32_t job_counter_sign(void)
{
  return (DB32(0x801be768) >> 31) & 1u;
}

/* ======== 0x801a0110 ======== */

/* chan_job_pump — channel-block job pump, registered by
 * cd_chan_block_boot via cd_chan_block_init(7,0x801a0110). When the job
 * counter reads non-negative: op_chan_fn_invoke(), and if the counter
 * then reads negative, arm flag @0x801febe8, move the 8-int8_t head pair
 * 0x801febd0 <- block+0/+4, clear the source pair, and jalr the fn ptr
 * @block+0x44 with (a0,a1) = the moved pair. Always bumps counters
 * block+0x50/+0x54. Disasm 0x801a0110-0x801a01b8. */
extern void op_chan_fn_invoke(void);
void chan_job_pump(void)
{
  uint8_t *blk = (uint8_t *)guest_at(0x801febe0);
  uint8_t *dst = (uint8_t *)guest_at(0x801febd0);
  if (job_counter_sign() == 0) {
    op_chan_fn_invoke();
    if (job_counter_sign() != 0) {
      uint32_t fn;
      DB32(0x801febe8) = 1;
      DB32(0x801febd0) = DB32(0x801febe0);
      DB32(0x801febd4) = DB32(0x801febe4);
      DB32(0x801febe0) = 0;
      DB32(0x801febe4) = 0;
      fn = *(uint32_t *)(blk + 0x44);
      if (fn != 0) {
        uint32_t a0 = *(uint32_t *)(dst + 0);
        uint32_t a1 = *(uint32_t *)(dst + 4);
        (void)call_guest(fn,a0,a1);
      }
    }
  }
  *(uint32_t *)(blk + 0x50) = *(uint32_t *)(blk + 0x50) + 1;
  *(uint32_t *)(blk + 0x54) = *(uint32_t *)(blk + 0x54) + 1;
}

/* ======== 0x801a3014 ======== */

/* cd_cmd_rec_init — command-record slot init (0x801a3014-0x801a3034).
 * Leaf: rec[55]=0x43 class tag, rec[44]=&rec[36] self-link stored as a
 * GUEST address, rec[36]=a1, rec[54]=1 armed (delay-slot store).
 * Disasm verified insn-for-insn. */
void cd_cmd_rec_init(uint32_t rec,uint32_t tag)
{
  uint8_t *r = (uint8_t *)guest_at(rec);
  r[55] = 0x43;
  *(uint32_t *)(r + 44) = rec + 36;
  r[36] = (uint8_t)tag;
  r[54] = 1;
}

/* ======== 0x801a3108 ======== */

/* cd_cmd_flush_a — gated double indirect-call + issue (0x801a3108-0x801a31dc).
 * When queue cursors BE7C0==BE7D0 and BE7BC armed: jalr BE7B0(rec,a1) then
 * jalr BE7AC(rec,a1) through the driver fn-ptr table. When BE800 armed:
 * jalr BE798(*(rec+12),a1) and jalr BE798(*(rec+12)+240,a1). Finally
 * jal 0x801a2184(rec, rec[55]!=0 ? rec[55] : 66); returns its v0.
 * jalr targets are runtime-loaded guest fn ptrs -> call_guest. */
uint32_t cd_cmd_flush_a(uint32_t rec,uint32_t a1)
{
  uint8_t *s0 = (uint8_t *)guest_at(rec);
  uint32_t sub, r;
  if (DB32(0x801be7c0) == DB32(0x801be7d0)) {
    if (DB32(0x801be7bc) != 0) {
      (void)call_guest(DB32(0x801be7b0),rec,a1);
      (void)call_guest(DB32(0x801be7ac),rec,a1);
    }
  }
  if (DB32(0x801be800) != 0) {
    sub = *(uint32_t *)(s0 + 12);
    (void)call_guest(DB32(0x801be798),sub,a1);
    sub = *(uint32_t *)(s0 + 12);
    (void)call_guest(DB32(0x801be798),sub + 240,a1);
  }
  r = s0[55];
  r = call_guest(0x801a2184u,rec,(r != 0) ? r : 66u);
  return r;
}

/* ======== 0x801a31e0 ======== */

/* cd_cmd_flush_b — flush variant issuing stat update (0x801a31e0-0x801a3288).
 * BE800 gate -> jalr BE798(*(rec+12)+480,a1) and +720. Then
 * jal 0x801a2184(rec, rec[55]!=0 ? 0 : BE7CC); negative result returns
 * as-is, else (v0&0xf)<<1 is stored to BE7F8 (32 when zero) and 0
 * returns. Disasm verified. */
uint32_t cd_cmd_flush_b(uint32_t rec,uint32_t a1)
{
  uint8_t *s0 = (uint8_t *)guest_at(rec);
  uint32_t sub, r, x;
  if (DB32(0x801be800) != 0) {
    sub = *(uint32_t *)(s0 + 12);
    (void)call_guest(DB32(0x801be798),sub + 480,a1);
    sub = *(uint32_t *)(s0 + 12);
    (void)call_guest(DB32(0x801be798),sub + 720,a1);
  }
  r = call_guest(0x801a2184u,rec,(s0[55] != 0) ? 0u : DB32(0x801be7cc));
  if ((int32_t)r < 0) return r;
  x = (r & 0xf) << 1;
  DB32(0x801be7f8) = x;
  if (x == 0) DB32(0x801be7f8) = 32;
  return 0;
}

/* ======== 0x801a328c ======== */

/* cd_cmd_issue_poll — status-poll + dual jalr + issue (0x801a328c-0x801a334c).
 * flag = (BE7CC!=0 && (*(u8*)*(rec+60))>>4 == 8 && rec[55]==0); stored to
 * BE804. flag==0 -> jalr BE78C(rec,a1). Always jalr BE788(rec,BE804) then
 * jal 0x801a2184(rec, ret&0xff); the tail compares are an identity
 * normalization — v0 returns unchanged in every path (verified: the
 * li -4 / move v0,v1 delay slots always leave v0==r). */
uint32_t cd_cmd_issue_poll(uint32_t rec,uint32_t a1)
{
  uint8_t *s0 = (uint8_t *)guest_at(rec);
  uint32_t flag = 0, r2, r;
  if (DB32(0x801be7cc) != 0) {
    uint32_t b = br2_ld8(*(uint32_t *)(s0 + 60));
    if ((b >> 4) == 8) flag = (s0[55] < 1) ? 1 : 0;
  }
  DB32(0x801be804) = flag;
  if (flag == 0) {
    (void)call_guest(DB32(0x801be78c),rec,a1);
  }
  r2 = call_guest(DB32(0x801be788),rec,DB32(0x801be804));
  r = call_guest(0x801a2184u,rec,r2 & 0xff);
  return r;
}

/* ======== 0x801a3850 ======== */

/* cd_cmd_rec_clear — conditional record scrub (0x801a3850-0x801a38b4).
 * Runs only when rec[73]!=0: clears status/tag fields and fills the
 * 6-int8_t aux window rec[93..98] with 0xff. Leaf, no calls. */
void cd_cmd_rec_clear(uint32_t rec)
{
  uint8_t *r = (uint8_t *)guest_at(rec);
  int i;
  if (r[73] == 0) return;
  r[73] = 0;
  r[70] = 0;
  *(uint16_t *)(r + 230) = 0;
  *(uint32_t *)(r + 20) = 0;
  *(uint32_t *)(r + 24) = 0;
  r[227] = 0;
  r[228] = 0;
  *(uint16_t *)(r + 230) = 0;
  r[233] = 0;
  r[234] = 0;
  *(uint32_t *)(r + 0) = 0;
  *(uint32_t *)(r + 4) = 0;
  *(uint32_t *)(r + 8) = 0;
  for (i = 0; i < 6; i++) r[93 + i] = 0xff;
}

/* ======== 0x801a39bc ======== */

/* cd_cmd_data_byte — indexed int8_t fetch (0x801a39bc-0x801a3a74).
 * idx = rec[69]-3 (u32 wrap), mode = rec[55].
 * mode==0: if idx<6 and aux rec[87+idx]==0 return 0; then if
 *   (int32)idx < (int32)rec[52] return int8_t at ptr40+idx else 0.
 * mode!=0 (0x4d and others share the tail): if (int32)idx <
 *   (int32)rec[54] return int8_t at ptr44+idx else 0.
 * slt is SIGNED — the wrapped idx must compare as int32 (verified vs
 * 0x801a3a08/0x801a3a34/0x801a3a54 slt insns). */
uint32_t cd_cmd_data_byte(uint32_t rec)
{
  uint8_t *r = (uint8_t *)guest_at(rec);
  uint32_t idx = (uint32_t)r[69] - 3u;
  uint32_t mode = r[55];
  uint32_t p;
  if (mode == 0) {
    /* Guest @0x801A39E4 uses signed slti(idx,6): idx wraps negative when
     * rec[69]<3, so it still takes the aux rec[87+idx] guard. */
    if ((int32_t)idx < 6 && r[idx + 87] == 0) return 0;
    if ((int32_t)idx < (int32_t)r[52]) {
      p = *(uint32_t *)(r + 40);
      return br2_ld8(p + idx);
    }
    return 0;
  }
  if ((int32_t)idx < (int32_t)r[54]) {
    p = *(uint32_t *)(r + 44);
    return br2_ld8(p + idx);
  }
  /* Guest: mode==0x4d returns 0xFF on out-of-bounds (sentinel, li @3A3C);
   * other nonzero modes return 0. (blocks 801A3A2C vs 801A3A4C) */
  return (mode == 0x4d) ? 0xffu : 0u;
}

/* ======== 0x801a3d84 ======== */

/* cd_cmd_state_commit — record state commit (0x801a3d84-0x801a3e90).
 * cmd = **(rec+60); when cmd==0xf3 and rec[232]!=0 and
 * (rec[70]==0xff || rec[73]==2): jalr BE784(rec,a1). Then dispatch on
 * rec[70]: ==1 -> cd_cmd_rec_init(rec,1); ==254 -> cd_cmd_rec_init(rec,0);
 * ==0 or ==255 -> bare return; others -> jalr rec[20](rec,a1) when nonzero
 * else jal op_tag_dispatch(rec,a1). Every exit zeroes v0. */
uint32_t cd_cmd_state_commit(uint32_t rec,uint32_t a1)
{
  uint8_t *s0 = (uint8_t *)guest_at(rec);
  uint32_t cmd, b70, fn;
  cmd = br2_ld8(*(uint32_t *)(s0 + 60));
  if (cmd == 0xf3 && s0[232] != 0 &&
      (s0[70] == 0xff || s0[73] == 2)) {
    (void)call_guest(DB32(0x801be784),rec,a1);
  }
  b70 = s0[70];
  if (b70 == 1) {
    (void)call_guest(0x801a3014u,rec,1);
    return 0;
  }
  if (b70 == 254) {
    (void)call_guest(0x801a3014u,rec,0);
    return 0;
  }
  if (b70 == 0 || b70 == 255) return 0;
  fn = *(uint32_t *)(s0 + 20);
  if (fn != 0) {
    (void)call_guest(fn,rec,a1);
  } else {
    (void)call_guest(0x801a24dcu,rec,a1);
  }
  return 0;
}

/* ======== 0x801a3e94 ======== */

/* cd_cmd_state_rx — command-record RX state machine (0x801a3e94-0x801a40f4).
 * cmd = **(rec+60). High nibble==0: datptr=*(rec+48) gets [0]=0xff,[1]=0,
 * rec[232]=rec[53]=0, jalr BE784(rec,a1) and return its v0.
 * Else nib=cmd>>4: a1 becomes old rec[232]; rec[232]=nib (delay-slot store,
 * restored when nib==15). datptr head reset + payload copy datptr[i]=cmd[i]
 * for i=2..rec[68]-1 (rec[53]=rec[68] stored unconditionally in the delay
 * slot). Then cmd[1]/rec[70]/rec[20]/rec[80] gating decides whether
 * BE784(rec,a1) fires before the rec[70] state dispatch:
 *   255 -> ret 255; byte55==0 -> ret b70-2; (b70-2)&0xff <252 &&
 *   cmd[0]!=0xf3 -> jalr BE784 ret; ==1 -> rec[71]=0,rec[70]=2 ret 2;
 *   ==0 -> rec[73]=1,rec[70]=1 ret 1; else jalr rec[24](rec,a1) or
 *   jal 0x801a2560, rec[70]+=ret, ret v0.
 * a1 register = param until 0x801a3efc, then old rec[232] (a1s). */
/* cd_cmd_state_rx — command-state RX machine (see body/labels). */
uint32_t cd_cmd_state_rx(uint32_t rec,uint32_t a1)
{
  uint8_t *s0 = (uint8_t *)guest_at(rec);
  uint32_t a1s = a1;
  uint32_t cmd, nib, b70, fn, r, i;
  uint32_t cmdptr, datptr;

  cmdptr = *(uint32_t *)(s0 + 60);
  cmd = br2_ld8(cmdptr);
  if ((cmd & 0xf0) == 0) {
    datptr = *(uint32_t *)(s0 + 48);
    br2_st8(datptr,0xff);
    br2_st8(datptr + 1,0);
    s0[232] = 0;
    s0[53] = 0;
    return call_guest(DB32(0x801be784),rec,a1s);
  }
  nib = cmd >> 4;
  a1s = s0[232];
  s0[232] = (uint8_t)nib;
  if (nib == 15) {
    s0[232] = (uint8_t)a1s;
    goto L_state;
  }
  datptr = *(uint32_t *)(s0 + 48);
  br2_st8(datptr,0);
  cmdptr = *(uint32_t *)(s0 + 60);
  datptr = *(uint32_t *)(s0 + 48);
  br2_st8(datptr + 1,br2_ld8(cmdptr));
  s0[53] = s0[68];
  if (2 < s0[68]) {
    for (i = 2; i < s0[68]; i++) {
      cmdptr = *(uint32_t *)(s0 + 60);
      datptr = *(uint32_t *)(s0 + 48);
      br2_st8(datptr + i,br2_ld8(cmdptr + i));
    }
  }
L_state:
  cmdptr = *(uint32_t *)(s0 + 60);
  if (br2_ld8(cmdptr + 1) != 0) goto L_notify_chk;
  b70 = s0[70];
  if (b70 != 1) goto L_aux_chk;
  if (*(uint32_t *)(s0 + 20) == 0) goto L_notify_chk;
L_aux_chk:
  if (s0[80] == 0) goto L_notify;
L_notify_chk:
  if (s0[232] == (uint8_t)a1s) goto L_dispatch;
L_notify:
  (void)call_guest(DB32(0x801be784),rec,a1s);
L_dispatch:
  b70 = s0[70];
  s0[74] = 0;
  if (b70 == 255) return 255;
  if (b70 != 0 && s0[55] == 0) return b70 - 2;
  if (((b70 - 2) & 0xff) < 252) {
    cmdptr = *(uint32_t *)(s0 + 60);
    if (br2_ld8(cmdptr) == 0xf3) goto L_b70_dispatch;
    return call_guest(DB32(0x801be784),rec,a1s);
  }
L_b70_dispatch:
  if (b70 == 1) {
    s0[71] = 0;
    s0[70] = 2;
    return 2;
  }
  if (b70 < 2) {
    s0[73] = 1;
    s0[70] = 1;
    return 1;
  }
  /* Guest: on the >=2 path, beq rec70,254 (@0x801A4078) -> block_801A40A8
   * stores 254 back to rec[70] (identity — already 254) and returns v0=254
   * with NO handler call. Do not run the rec[24]/0x801a2560 path. */
  if (b70 == 254) return 254;
  fn = *(uint32_t *)(s0 + 24);
  if (fn != 0) {
    r = call_guest(fn,rec,a1s);
  } else {
    r = call_guest(0x801a2560u,rec,a1s);
  }
  s0[70] = (uint8_t)(s0[70] + r);
  return r;
}

/* ======== 0x801a3a78 ======== */

/* cd_cmd_rec_scan — record scan/match pump (0x801a3a78-0x801a3cf0).
 * cd_bzero(rec+87,6) first. Data path (lhu rec[230]!=0 && rec[40]!=0 &&
 * rec[233]!=0): t1 = rec[52]<7 ? 6 : rec[52]; for t0 in 0..rec[233)-1,
 * records are 5-int8_t slots at *(rec+4)+t2 (t2+=5): flag int8_t +2 picks
 * a3 (0xff when set else 1); inner pass matches aux rec[93+i]==t0 against
 * stream[i]&a3; on match the BE7C8 index int8_t (+3) is added to BE7C8 when
 * <61 and aux rec[87+i]=1 is marked for every match. No-data path
 * (3bf0): rec[232] in {4,5,7} && rec[230]==0 && rec[52]>=2 &&
 * (*(rec+40)&0xc0)==0x40 && (*(rec+40)+1)&1 && BE7C8+10<61 ->
 * rec[87]=rec[88]=1, BE7C8+=10; else rec[232]==3 -> rec[87]=1;
 * else rec[230]==0 -> rec[87..92]=1. v0 return is dead in callers. */
/* cd_cmd_rec_scan — command record scan machine (see body/labels). */
uint32_t cd_cmd_rec_scan(uint32_t rec)
{
  uint8_t *s0 = (uint8_t *)guest_at(rec);
  uint32_t t0, t1, t2, a2, a3, v0, v1, i;
  uint32_t aux93, aux87, stream, base;

  (void)call_guest(0x801a09b0u,rec + 87,6);   /* cd_bzero(rec+87,6) */
  v0 = *(uint16_t *)(s0 + 230);
  if (v0 != 0 && *(uint32_t *)(s0 + 40) != 0) {
    t1 = (s0[52] < 7) ? s0[52] : 6;
    if (s0[233] == 0) return 0;
    aux93 = rec + 93;
    aux87 = rec + 87;
    t0 = 0; t2 = 0;
    do {
      stream = *(uint32_t *)(s0 + 40);
      base = *(uint32_t *)(s0 + 4);
      a3 = (br2_ld8(base + t2 + 2) != 0) ? 0xff : 1;
      a2 = 0;
      if (t1 != 0) {
        for (i = 0; i < t1; i++) {
          if (br2_ld8(aux93 + i) == t0 &&
              (br2_ld8(stream + i) & a3) != 0) {
            a2 = 1;
            break;
          }
        }
      }
      if (a2 != 0) {
        base = *(uint32_t *)(s0 + 4);
        v1 = br2_ld8(base + t2 + 3) + DB32(0x801be7c8);
        if (v1 < 61) {
          DB32(0x801be7c8) = v1;
        } else {
          a2 = 0;
        }
      }
      if (a2 != 0) {
        for (i = 0; i < t1; i++) {
          if (br2_ld8(aux93 + i) == t0) br2_st8(aux87 + i,1);
        }
      }
      t0++;
      t2 += 5;
    } while (t0 < s0[233]);
    return 0;
  }
  /* no-data path (0x801a3bf0) */
  v1 = s0[232];
  if (!((v1 - 4) < 2u || v1 == 7)) goto L_c4;
  if (*(uint16_t *)(s0 + 230) != 0) goto L_c4;
  if (s0[52] < 2) goto L_c4;
  stream = *(uint32_t *)(s0 + 40);
  if ((br2_ld8(stream) & 0xc0) != 0x40) return (br2_ld8(stream) & 0xc0);
  if ((br2_ld8(stream + 1) & 1) == 0) return 0;
  v0 = DB32(0x801be7c8) + 10;
  if (v0 >= 61) return 0;
  s0[88] = 1;
  s0[87] = 1;
  DB32(0x801be7c8) = v0;
  return v0;
L_c4:
  if (s0[232] == 3) {
    s0[87] = 1;
    return 1;
  }
  if (*(uint16_t *)(s0 + 230) != 0) return *(uint16_t *)(s0 + 230);
  for (i = 0; i < 6; i++) s0[87 + (5 - i)] = 1;
  return 1;
}

/* ======== 0x801a189c ======== */

/* cd_cmd_queue_service — command-queue service pass (0x801a189c-0x801a1a28).
 * Bit1 of *(BE7E4)+10 set -> clear it and return. Else BE7E8=1 (delay-slot
 * store, unconditional), saturating +1 bumps on counters 0x801fefd0
 * (when BE7D0!=0) and 0x801fefd4 (when BE7D4==0), gated by BE7B8.
 * When BE7D4>=BE7D0: BE7C4=0, BE7C0=BE7D0, jal 0x801a1b50(table+BE7D0*
 * 240); on 0 result jalr BE780(0xffff,a1). BE7C8=0 then drain loop:
 * while BE7D4>=BE7C0 jal 0x801a1e84(table+BE7C0*240). Finally
 * sh 0x88 -> *(BE7E4)+14. Returns 0 on every path. */
uint32_t cd_cmd_queue_service(uint32_t a0,uint32_t a1)
{
  uint8_t *q;
  uint32_t v0, v1, r;
  (void)a0;
  q = (uint8_t *)guest_at(DB32(0x801be7e4));
  v0 = *(uint16_t *)(q + 10) & 2;
  if (v0 != 0) {
    *(uint16_t *)(q + 10) = 0;
    return 0;
  }
  v1 = DB32(0x801be7d0);
  DB32(0x801be7e8) = 1;
  if (v1 != 0) {
    v0 = DB32(0x801fefd0);
    if ((int32_t)v0 < 150) DB32(0x801fefd0) = v0 + 1;
  }
  if (DB32(0x801be7d4) == 0) {
    v0 = DB32(0x801fefd4);
    if ((int32_t)v0 < 150) DB32(0x801fefd4) = v0 + 1;
  }
  if (DB32(0x801be7b8) == 0) return 0;
  if ((int32_t)DB32(0x801be7d4) < (int32_t)DB32(0x801be7d0)) return 0;
  v1 = DB32(0x801be7d0);
  DB32(0x801be7c4) = 0;
  DB32(0x801be7c0) = v1;
  r = call_guest(0x801a1b50u,DB32(0x801be7b4) + v1 * 240,a1);
  if (r == 0) {
    (void)call_guest(DB32(0x801be780),0xffff,a1);
  }
  DB32(0x801be7c8) = 0;
  if ((int32_t)DB32(0x801be7d4) >= (int32_t)DB32(0x801be7c0)) {
    do {
      v1 = DB32(0x801be7c0);
      (void)call_guest(0x801a1e84u,DB32(0x801be7b4) + v1 * 240,a1);
    } while ((int32_t)DB32(0x801be7d4) >= (int32_t)DB32(0x801be7c0));
  }
  q = (uint8_t *)guest_at(DB32(0x801be7e4));
  *(uint16_t *)(q + 14) = 0x88;
  return 0;
}

/* ======== 0x801a38b8 ======== */

/* cd_cmd_queue_drain — queue drain loop (0x801a38b8-0x801a39a8).
 * Per iteration on head=BE7C0, rec=0x801ff080+head*240: selector -9
 * skips pre-ops; selector!=0 -> op_frame_advance(rec)+qbyte_take(rec);
 * selector==0 -> BE7D8[head]=0. Then BE7C4=0, sh 0 -> *(BE828)+10,
 * BE7C0++ and either return 1 when BE7D4<new head, or
 * jal 0x801a1b50(0x801ff080+newhead*240) looping while it returns 0.
 * a1 becomes 0xffff after the first iteration (delay-slot li). */
uint32_t cd_cmd_queue_drain(uint32_t sel)
{
  uint32_t a1 = sel;
  uint32_t v0, head, rec, p;
  for (;;) {
    head = DB32(0x801be7c0);
    rec = 0x801ff080u + head * 240u;
    if (a1 == 0xfffffff7u) {
      /* selector -9: skip pre-ops */
    } else if (a1 != 0) {
      (void)call_guest(0x801a40f8u,rec,a1);
      (void)call_guest(0x801a39acu,rec,a1);
    } else {
      DB32(0x801be7d8u + head * 4u) = 0;
    }
    p = DB32(0x801be828);
    DB32(0x801be7c4) = 0;
    br2_st16(p + 10,0);
    head = DB32(0x801be7c0) + 1;
    DB32(0x801be7c0) = head;
    v0 = DB32(0x801be7d4);
    if ((int32_t)v0 < (int32_t)head) {
      a1 = 0xffff;
      return 1;
    }
    rec = 0x801ff080u + head * 240u;
    v0 = call_guest(0x801a1b50u,rec,a1);
    a1 = 0xffff;
    if (v0 != 0) return v0;
  }
}
