/* cd.c — CD/file services: async read pump, status pollers,
 * command issuer with retry (verbatim Ghidra, decomp/batch_sm3.txt).
 *
 * Guest globals via g_* arena bindings; timer cd_timer_read and
 * channel ops mc_session/95d78/961a8/961cc/98590/95ec8/98908/
 * 96e14/98c90 remain extern (deeper CD-driver cluster). */
#include <stdint.h>
#include <string.h>
#include <stdio.h>

typedef uint8_t  undefined;
typedef uint8_t  undefined1;
typedef uint16_t undefined2;
typedef uint32_t undefined4;
typedef int8_t   byte;
typedef uint16_t ushort;
typedef uint32_t uint;
#ifndef bool
#define bool int
#define true 1
#define false 0
#endif

extern void *guest_at(uint32_t addr);
extern uint32_t guest_sp(void);
extern uint32_t psx_read_word(uint32_t addr);
extern void psx_write_word(uint32_t addr, uint32_t val);

uint cd_timer_read(int);
void cd_busy_wait_fwd(int);
int cd_cmd_dispatch3(undefined4);
void cd_init_done_cb(void);
undefined4 mc_session(undefined4);
undefined4 mc_session_b(undefined4);
undefined4 cd_cmd_issue_async(byte,int);
undefined4 cd_read_open(int);
undefined4 cd_cmd_issue(byte,uint32_t,uint32_t,int);
bool cd_read_setup(undefined4,undefined4,uint);
int cd_chan_boot_init(int);
void cd_result_wait_b_fwd(int,undefined4);
void printf_forward(const char *, ...);
extern void br2_irq_poll(void);
extern uint32_t guest_of(const void *);
extern uint8_t  br2_ld8p(uint32_t cell);
extern uint16_t br2_ld16p(uint32_t cell);
extern uint32_t br2_ld32p(uint32_t cell);

#define CDDBG_INC(a) do{ (*(volatile uint32_t *)guest_at(a))++; }while(0)
#define CDDBG_SET(a,v) do{ *(volatile uint32_t *)guest_at(a)=(uint32_t)(v); }while(0)

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

/* MMIO register-block accessors. The pointer cells 0x801bdeb0,
 * 0x801bdec0 and 0x801bdec4 resolve to HARDWARE addresses — the SPU
 * register file (0x1F801C00+), DPCR (0x1F8010F0) and 0x1F801014.
 * guest_at() can only back RAM/scratchpad; an MMIO address lands in the
 * open-bus mirror so the access never reaches the device (the same
 * mistranslation class as the GPU/DMA pointer cells fixed earlier).
 * Every register-block access must go through br2_ldN/br2_stN on the
 * resolved guest address. idx is the u16 index the original b[idx]
 * accesses used (byte offset = idx*2). */
#define CDMMIO16(base, idx)        br2_ld16((base) + ((uint32_t)(idx) << 1))
#define CDMMIO16_ST(base, idx, v)  \
    br2_st16((base) + ((uint32_t)(idx) << 1), (uint16_t)(v))
#define CDMMIO32(base, idx)        br2_ld32((base) + ((uint32_t)(idx) << 2))
#define CDMMIO32_ST(base, idx, v)  \
    br2_st32((base) + ((uint32_t)(idx) << 2), (uint32_t)(v))

/* ---- guest globals ---- */
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

#define DAT_801bc7b4   (*(uint8_t  *)g_DAT_801bc7b4)
#define DAT_801bcb80   (*(int32_t  *)g_DAT_801bcb80)
#define DAT_801bcb94   (*(int32_t  *)g_DAT_801bcb94)
#define DAT_801bcb98   (*(int32_t  *)g_DAT_801bcb98)
#define DAT_801bcb9c   (*(int32_t  *)g_DAT_801bcb9c)
#define DAT_801bcba4   (*(int32_t  *)g_DAT_801bcba4)
#define DAT_801bcba8   (*(int32_t  *)g_DAT_801bcba8)
#define DAT_801bcbac   (*(int32_t  *)g_DAT_801bcbac)
#define DAT_801bcbb0   (*(int32_t  *)g_DAT_801bcbb0)
#define DAT_801bc854   (*(uint32_t *)g_DAT_801bc854)
#define DAT_801bc848   (*(uint32_t *)g_DAT_801bc848)
#define DAT_801bc7c0   ((int32_t  *)g_DAT_801bc7c0)

/* ======== 0x8018d150 ======== */

undefined4 cd_mode_flag_set(byte,int,undefined4);
undefined4 cd_cmd_issue(byte,uint32_t,uint32_t,int);

undefined4 cd_read_issue(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  /* IRQ/coroutine-boundary function: clean-C body cannot supply the
   * live guest register file the deferred CD coroutine needs, and its
   * MMIO busy-waits lack per-block interrupt checks. Delegate to the
   * generated MIPS (behavioral oracle) — same contract as the other
   * read-path functions already delegated. */
  return (undefined4)br2_call_generated(0x8018d150u,param_1, param_2, param_3, 0);
}

/* ======== 0x80198af0 ======== */

/* cd_init_session — CD read-session init + async-Pause handshake (0x80198af0).
 * Waits on DAT_801bcba4 (cleared by session cb 0x80198590 = cd_init_done_cb,
 * registered as the INT2 command-complete callback via mc_session->
 * DAT_801bc848). It issues Pause(0x09) once via cd_cmd_issue_async, sets
 * DAT_801bcba4=1, then waits <=0x79 frame ticks (cd_timer_read -> 0x801bdde0
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
void cd_init_session(void)
{
  br2_call_generated(0x80198af0, 0, 0, 0, 0);
}

/* ======== 0x80198e2c ======== */

int cd_read_poll(int param_1,undefined4 param_2)

{
  /* IRQ/coroutine-boundary function: clean-C body cannot supply the
   * live guest register file the deferred CD coroutine needs, and its
   * MMIO busy-waits lack per-block interrupt checks. Delegate to the
   * generated MIPS (behavioral oracle) — same contract as the other
   * read-path functions already delegated. */
  return (int)br2_call_generated(0x80198e2cu,param_1, param_2, 0, 0);
}

/* ======== 0x80195d8c ======== */

extern int cd_stamp_to_sector(const void *);
undefined4 cd_mode_flag_set(byte param_1,int param_2,undefined4 param_3)

{
  /* CD command issuer on the direct cd_file_load_sm path: its retry loop
   * issues commands whose INT2 completion is delivered by the deferred CD
   * coroutine that needs the live guest register file — the same boundary
   * contract as the other read-path functions. Delegate to generated. */
  return (undefined4)br2_call_generated(0x80195d8cu,(uint32_t)param_1,(uint32_t)param_2,param_3,0);
}

/* ======== 0x8019b200 ======== */

void cd_chan_boot_fwd(void)

{
  cd_chan_boot_init(0);
  return;
}

#define DAT_801bcb84   (*(uint32_t *)guest_at(0x801bcb84))
#define DAT_801bcb88   (*(int32_t  *)guest_at(0x801bcb88))
#define DAT_801bcb8c   (*(uint32_t *)guest_at(0x801bcb8c))
#define DAT_801bcb90   (*(uint32_t *)guest_at(0x801bcb90))
#define DAT_801bc84c   (*(uint32_t *)guest_at(0x801bc84c))
#define DAT_801bcba0   (*(uint32_t *)guest_at(0x801bcba0))

/* ======== batch26: CD driver cluster ======== */

extern uint cd_result_wait_a(int,undefined1 *);
extern void cd_str_print(uint32_t);
extern void cd_cmd_state_reset(void);
extern undefined2 cd_intc_busy_read(void);
extern uint32_t cd_response_process(void);
extern undefined1 cd_status_raw(void);
extern bool cd_cmd_issue_sync(byte,int,undefined4);
extern undefined1 * cd_param_buf_get(void);
extern void cd_cmd_reset_wrap(void);
extern undefined1 cd_mode_byte_read(void);
extern int cd_stamp_to_sector(const void *);
extern void vsync_wait_n(int,int);
extern uint cd_result_wait_b(int,undefined1 *);
extern undefined4 cd_busy_wait(int);
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

/* cd_cmd_issue — CD command issue. param_2/param_3 are guest addrs
 * (or 0); param_4 = async flag. */
undefined4 cd_cmd_issue(byte param_1,uint32_t param_2,uint32_t param_3,int param_4)

{
  /* The sync wait loop (DB8(0x801bcb24)==0) depends on the CD interrupt
   * handler setting the intc-busy flag that gates cd_response_process().
   * A clean C spin cannot be interrupted at guest-instruction granularity,
   * so the IRQ that would set busy is either deferred forever or abandons
   * this frame. Run the recompiled guest body so the wait is interruptible
   * exactly like hardware. Clean port preserved in git history. */
  return (undefined4)br2_call_generated(0x80196e14,(uint32_t)param_1,param_2,param_3,(uint32_t)param_4);
}

/* ======== 0x80198c90 ======== */

bool cd_read_setup(undefined4 param_1,undefined4 param_2,uint param_3)

{
  /* Read configurator: picks sector size (0x200/0x246/0x249), mode|0x20,
   * buffer ptrs, registers the INT2/step callbacks via mc_session(_b),
   * arms the 0x79 timeout and issues Pause when the drive reports busy.
   * A wrong sector-size/mode/buffer or a callback registered against the
   * wrong slot makes the read deliver the wrong amount -> the consumer
   * stops mid-sector and the completion event never fires (0x60a wedge).
   * It also sits on the IRQ/deferred-coroutine boundary like the other
   * read-path functions already delegated. Delegate to generated. */
  return (bool)br2_call_generated(0x80198c90u,(uint32_t)param_1,
                                  (uint32_t)param_2,(uint32_t)param_3,0);
}

#if 0
  int iVar1;
  int iVar2;
  uint uVar3;

  if (DAT_801bcba4 != 0) {
    iVar1 = cd_timer_read(0xffffffff);
    do {
      if (DAT_801bcba4 == 0) goto LAB_80198d0c;
      iVar2 = cd_timer_read(0xffffffff);
    } while ((uint)(iVar2 - iVar1) < 0x79);
    mc_session(DAT_801bcba8);
    DAT_801bcba4 = 0;
  }
LAB_80198d0c:
  if ((param_3 & 0x30) == 0) {
    DAT_801bcb90 = 0x200;
  }
  else if ((param_3 & 0x30) == 0x20) {
    DAT_801bcb90 = 0x249;
  }
  else {
    DAT_801bcb90 = 0x246;
  }
  DAT_801bcb8c = param_3 | 0x20;
  DAT_801bcb80 = param_1;
  DAT_801bcb84 = param_2;
  DAT_801bcba8 = mc_session(0);
  DAT_801bcbac = mc_session_b(0);
  if ((DAT_801bc7b4 & 1) != 0) {
    DAT_801bcbb0 = cd_cmd_dispatch3(0);
  }
  DAT_801bcb9c = cd_timer_read(0xffffffff);
  uVar3 = cd_status_raw();
  if ((uVar3 & 0xe0) != 0) {
    cd_cmd_issue_sync(9,0,0);
  }
  iVar1 = cd_read_open(0);
  return 0 < iVar1;
}
#endif

/* ======== 0x8019a460 ======== */

/* cd_timer_read — frame/timer read + DAT_801bdde0 waits (0x8019a460).
 * This is the resumable half of the same deferred-IRQ contract as
 * cd_chan_full_init/cd_intc_dispatch: its vsync_wait_n spins must let the CD
 * IRQ drive cd_ctx_restore -> cd_intc_dispatch -> cd_cb8_dispatch (which
 * increments DAT_801bdde0). That save/restore operates on cpu->gpr, which a
 * clean-C body does not own — the host stack is invisible to it, so the
 * deferred coroutine cannot resume and the frame counter freezes (observed:
 * DAT_801bdde0 stalls -> vsync_wait_n timeout -> BIOS-tramp crash). The whole
 * function therefore runs generated; the thunk keeps direct C callers correct
 * and makes gen_clean_table skip the address. */
uint cd_timer_read(int param_1)
{
  return (uint)br2_call_generated(0x8019a460u, (uint32_t)param_1, 0, 0, 0);
}

/* ======== 0x80195d44 ======== */

void cd_result_wait_b_fwd(int param_1,undefined4 param_2)

{
  cd_result_wait_b(param_1,(undefined1 *)(uintptr_t)param_2);
  return;
}

/* ======== 0x801961cc ======== */

void cd_busy_wait_fwd(int param_1)

{
  cd_busy_wait(param_1);
  return;
}

/* ======== 0x801961a8 ======== */

int cd_cmd_dispatch3(undefined4 param_1)

{
  return cd_dispatch_tbl4(3,param_1);
}

/* ======== 0x80195d64 ======== */

undefined4 mc_session(undefined4 param_1)

{
  undefined4 uVar1;
  
  uVar1 = DAT_801bc848;
  DAT_801bc848 = param_1;
  return uVar1;
}

/* ======== 0x80195d78 ======== */

undefined4 mc_session_b(undefined4 param_1)

{
  undefined4 uVar1;
  
  uVar1 = DAT_801bc84c;
  DAT_801bc84c = param_1;
  return uVar1;
}

/* ======== 0x80195ec8 ======== */

undefined4 cd_cmd_issue_async(byte param_1,int param_2)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  
  CDDBG_INC(0x801bce7c);
  CDDBG_SET(0x801bce80, param_1);
  uVar1 = DAT_801bc848;
  iVar3 = 3;
  while( true ) {
    DAT_801bc848 = 0;
    if ((param_1 != 1) && ((DAT_801bc854 & 0x10) != 0)) {
      cd_cmd_issue(1,0,0,0);
    }
    if ((((param_2 == 0) || (I32AT(0x801bc7c0,(uint)param_1) == 0)) ||
        (iVar2 = cd_cmd_issue(2,param_2,0,0), iVar2 == 0)) &&
       (DAT_801bc848 = uVar1, iVar2 = cd_cmd_issue(param_1,param_2,0,1), iVar2 == 0)) break;
    iVar3 = iVar3 + -1;
    if (iVar3 == -1) {
      DAT_801bc848 = uVar1;
      return 0;
    }
  }
  return 1;
}

/* ======== 0x80198908 ======== */

/* cd_read_open — issues Pause/SetLoc/Setmode/ReadN for a read session.
 * The clean port builds the Setmode param byte on a guest stack buffer via
 * guest_sp()-0x18; a stale/wrong guest_sp makes cd_mode_flag_set(0xe,..)
 * hand generated cd_cmd_issue a buffer that decodes to 0x1C instead of the
 * computed 0xA0 mode, so the drive stays in user-data mode (mode_reg&0x20==0)
 * and never delivers the per-sector MSF stamp that cd_read_step_cb verifies
 * -> "sector error" -> DAT_801bcb94=-1 -> cd_read_open(1) retry livelock on
 * the first file's first sectors. The generated body keeps sp live and has
 * per-block interrupt checks. Delegate it. */
undefined4 cd_read_open(int param_1)
{
  return (undefined4)br2_call_generated(0x80198908u,(uint32_t)param_1,0,0,0);
}

#if 0
undefined4 cd_read_open_clean(int param_1)

{
  uint uVar1;
  undefined4 uVar2;
  int iVar3;
  uint uVar4;
  uint8_t *p18;
  
  CDDBG_INC(0x801bce64);
  mc_session(0);
  mc_session_b(0);
  if ((DAT_801bc7b4 & 1) != 0) {
    cd_cmd_dispatch3(0);
  }
  uVar1 = cd_status_raw();
  if ((uVar1 & 0x10) != 0) {
    CDDBG_INC(0x801bce68);   /* status bit4 early-return */
    uVar1 = cd_timer_read(0xffffffff);
    if ((uVar1 & 0x3f) == 0) {
      cd_str_print(0x80125ae0);
    }
    cd_cmd_issue_async(1,0);
    DAT_801bcb9c = cd_timer_read(0xffffffff);
    DAT_801bcb94 = 0xffffffff;
    return 0xffffffff;
  }
  if (param_1 != 0) {
    cd_str_print(0x80125af8);
    cd_mode_flag_set(9,0,0);
    /* cd_mode_flag_set -> cd_cmd_issue treats param_2 as a guest address;
     * cd_param_buf_get returns a host pointer into guest RAM. */
    uVar2 = guest_of(cd_param_buf_get());
    iVar3 = cd_mode_flag_set(2,uVar2,0);
    if (iVar3 == 0) {
      CDDBG_INC(0x801bce74);   /* mode_flag_set(2) fail */
      DAT_801bcb94 = 0xffffffff;
      return 0xffffffff;
    }
  }
  cd_cmd_reset_wrap();
  /* stack buf = sp+0x10 (frame 0x28) -> guest_sp()-0x18; verified
   * `sb $s0,0x10($sp)` / `addiu $a1,$sp,0x10` in 0x80198908. */
  p18 = (uint8_t *)guest_at((uint32_t)(guest_sp() - 0x18));
  p18[0] = (undefined1)DAT_801bcb8c;
  uVar4 = DAT_801bcb8c & 0xff;
  uVar1 = cd_mode_byte_read();
  if (((uVar4 != uVar1) || (param_1 != 0)) &&
      (iVar3 = cd_mode_flag_set(0xe,(int)(guest_sp() - 0x18),0), iVar3 == 0)) {
    CDDBG_INC(0x801bce78);   /* mode_flag_set(0xe) fail */
    DAT_801bcb94 = 0xffffffff;
    return 0xffffffff;
  }
  DAT_801bcba0 = cd_stamp_to_sector(cd_param_buf_get());
  mc_session_b(0x801985c4);
  if ((DAT_801bc7b4 & 1) != 0) {
    cd_cmd_dispatch3(0x80198838);
  }
  DAT_801bcb88 = DAT_801bcb84;
  CDDBG_INC(0x801bce6c);              /* reached ReadN issue */
  CDDBG_SET(0x801bce70, DAT_801bcb80);/* sector count bcb80 */
  cd_cmd_issue_async(6,0);
  DAT_801bcb94 = DAT_801bcb80;
  DAT_801bcb98 = cd_timer_read(0xffffffff);
  return DAT_801bcb94;
}
#endif /* cd_read_open clean body preserved for reference */

/* ======== 0x80198590 ======== */

void cd_init_done_cb(void)

{
  mc_session(DAT_801bcba8);
  DAT_801bcba4 = 0;
  return;
}

/* ======== batch27: CD driver core ======== */

extern int  console_emit(int);
extern void console_pct(void);
extern int  cd_sync_wait_b();
extern int  cd_sync_wait_c();
extern void bios_table_tramp();
extern void bios_kobj_tramp();
extern uint16_t cd_u16_swap_store(uint16_t);
extern uint32_t gpu_status_cmd(uint);

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

uint cd_result_wait_a(int param_1,undefined1 *param_2)

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
  return (uint)br2_call_generated(0x801968ccu, (uint32_t)param_1,
                                  (uint32_t)(uintptr_t)param_2, 0, 0);
}

/* ======== 0x80197ad0 ======== */

void cd_str_print(uint32_t param_1)

{
  char cVar1;
  uint8_t *s;
  
  s = param_1 ? (uint8_t *)guest_at(param_1) : (uint8_t *)guest_at(0x801258c8);
  while( true ) {
    cVar1 = (char)*s;
    s = s + 1;
    if (cVar1 == '\0') break;
    console_emit((int)cVar1);
  }
  console_pct();
  return;
}

/* ======== 0x801972a8 ======== */

void cd_cmd_state_reset(void)

{
  /* IRQ-poll wait for the CD response channel (0x801bcb18&7) to drain —
   * drained only by the IRQ-driven cd_response_process, which a clean-C
   * spin cannot resume. Delegate to generated. */
  br2_call_generated(0x801972a8u,0,0,0,0);
  return;
}

/* ======== 0x8019a7d4 ======== */

undefined2 cd_intc_busy_read(void)

{
  return DB16(0x801bcd1a);
}

/* ======== 0x80196370 ======== */

undefined4 cd_response_process(void)

{
  /* 1372-byte response state machine — IRQ-context coroutine semantics and
   * per-type dispatch are exactly reproduced only by the recompiled body.
   * Delegate to generated (same contract as the other IRQ-wait CD fns). */
  return (undefined4)br2_call_generated(0x80196370u,0,0,0,0);
}

/* ======== small getters ======== */

undefined1 cd_status_raw(void)

{
  return (undefined1)DAT_801bc854;
}

undefined1 cd_mode_byte_read(void)

{
  return DB8(0x801bc864);
}

undefined1 * cd_param_buf_get(void)

{
  return (undefined1 *)guest_at(0x801bc860);
}

void cd_cmd_reset_wrap(void)

{
  cd_cmd_state_reset();
  return;
}

/* ======== 0x80195ffc ======== */

bool cd_cmd_issue_sync(byte param_1,int param_2,undefined4 param_3)

{
  undefined4 uVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  uVar1 = DAT_801bc848;
  iVar5 = 3;
  CDDBG_INC(0x801bce20);
  do {
    DAT_801bc848 = 0;
    if ((param_1 != 1) && ((DAT_801bc854 & 0x10) != 0)) {
      cd_cmd_issue(1,0,0,0);
    }
    if (((param_2 == 0) || (I32AT(0x801bc7c0,(uint)param_1) == 0)) ||
       (iVar3 = cd_cmd_issue(2,param_2,param_3,0), iVar3 == 0)) {
      DAT_801bc848 = uVar1;
      iVar3 = cd_cmd_issue(param_1,param_2,param_3,0);
      iVar4 = 0;
      if (iVar3 == 0) break;
      CDDBG_INC(0x801bce24);   /* cd_cmd_issue(param_1) returned nonzero */
      CDDBG_SET(0x801bce28,iVar3);
    }
    iVar5 = iVar5 + -1;
    iVar4 = -1;
    DAT_801bc848 = uVar1;
  } while (iVar5 != -1);
  if (iVar4 == 0) {
    iVar5 = cd_result_wait_a(0,(undefined1 *)(uintptr_t)param_3);
    bVar2 = iVar5 == 2;
    CDDBG_SET(0x801bce2c,iVar5);
  }
  else {
    bVar2 = false;
  }
  return bVar2;
}

/* ======== 0x801962f0 ======== */

int cd_stamp_to_sector(const void *param_1)

{
  const uint8_t *s = (const uint8_t *)param_1;
  return (((uint)(s[0] >> 4) * 10 + (s[0] & 0xf)) * 0x3c +
         (uint)(s[1] >> 4) * 10 + (s[1] & 0xf)) * 0x4b +
         (uint)(s[2] >> 4) * 10 + (s[2] & 0xf) + -0x96;
}

/* ======== 0x80196b4c ======== */

uint cd_result_wait_b(int param_1,undefined1 *param_2)

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
  return (uint)br2_call_generated(0x80196b4cu, (uint32_t)param_1,
                                  (uint32_t)(uintptr_t)param_2, 0, 0);
}

/* ======== 0x80197698 ======== */

undefined4 cd_busy_wait(int param_1)

{
  /* IRQ/coroutine-boundary function: clean-C body cannot supply the
   * live guest register file the deferred CD coroutine needs, and its
   * MMIO busy-waits lack per-block interrupt checks. Delegate to the
   * generated MIPS (behavioral oracle) — same contract as the other
   * read-path functions already delegated. */
  return (undefined4)br2_call_generated(0x80197698u,param_1, 0, 0, 0);
}

/* ======== 0x8019a6e0 ======== */

int cd_dispatch_tbl4(int param_1,uint32_t param_2)

{
  /* IRQ/coroutine-boundary function: clean-C body cannot supply the
   * live guest register file the deferred CD coroutine needs, and its
   * MMIO busy-waits lack per-block interrupt checks. Delegate to the
   * generated MIPS (behavioral oracle) — same contract as the other
   * read-path functions already delegated. */
  return (int)br2_call_generated(0x8019a6e0u,param_1, param_2, 0, 0);
}

/* ======== 0x801985c4 ======== */

void cd_read_step_cb(uint32_t param_1,undefined4 param_2)

{
  /* Per-sector read-step callback: MSF stamp-verify into a guest stack buf,
   * buffer advance, sync-wait issue of the next sector, and the read-complete
   * Pause + user callback. Its stack-buf address is guest_sp()-0x20, which only
   * reproduces the MIPS frame if the clean frame model is exact; a mismatch
   * reads a wrong stamp -> "sector error" -> abort -> the completion event
   * never fires -> TestEvent spins (the 0x60a wedge). It also drives the
   * cd_sync_wait and cd_cmd_issue helpers which cross the deferred-IRQ
   * coroutine boundary. Delegate to generated for a faithful SP/frame + IRQ path.
   * param_1 taken as a full word (was char) so a0 is not truncated. */
  br2_call_generated(0x801985c4u,param_1,param_2,0,0);
  return;
}

/* ======== 0x80198838 ======== */

void cd_read_finish_cb(void)

{
  DAT_801bcb88 = DAT_801bcb88 + DAT_801bcb90 * 4;
  DAT_801bcb94 = DAT_801bcb94 + -1;
  DAT_801bcba0 = DAT_801bcba0 + 1;
  if (DAT_801bcb94 == 0) {
    mc_session_b(DAT_801bcbac);
    if ((DAT_801bc7b4 & 1) != 0) {
      cd_cmd_dispatch3(DAT_801bcbb0);
    }
    mc_session(0x80198590);
    cd_cmd_issue_async(9,0);
    DAT_801bcba4 = 1;
    if (CDB32G(0x801bc7b0) != 0) {
      call_guest(CDB32G(0x801bc7b0),2,CDB32(0x801bcbb4));
    }
  }
  return;
}

/* ---- batch28: CD sync-wait wrappers + BIOS trampolines ---- */
extern uint32_t cd_cmd_issue_b(uint32_t,uint32_t);
extern uint32_t cd_cmd_issue_c(uint32_t,uint32_t);
uint32_t bios_call(uint32_t fn, uint32_t a0);
extern void cd_cmd_bytes(uintptr_t);

uint cd_sync_wait_a(int param_1,undefined1 *param_2)
{
  return cd_result_wait_a(param_1,param_2);
}

int cd_sync_wait_b(uint32_t param_1,uint32_t param_2)
{
  return cd_cmd_issue_b(param_1,param_2) == 0;
}

int cd_sync_wait_c(uint32_t param_1,uint32_t param_2)
{
  return cd_cmd_issue_c(param_1,param_2) == 0;
}

/* BIOS table trampolines — guest stubs are `li t2,<vec>; jr t2;
 * li t1,<svc>` forwarding caller a0-a3. Disasm-verified:
 * 0x80199fe0 = B0/0x5b, 0x8019a670 = C0/0x0a, 0x801957f0 = A0/0x44. */
extern void br2_bios_tramp(uint32_t,uint32_t);
extern uint32_t br2_call_generated(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
void bios_table_tramp()
{
  br2_bios_tramp(0xb0,0x5b);
}

void bios_kobj_tramp()
{
  br2_bios_tramp(0xc0,0x0a);
}

/* ---- batch29: CD command submit + status ---- */

/* cd_cmd_issue_b — CD command issue A (param_1/param_2 -> regs 48/4c),
 * polls status bit 0x40 then waits DMA-clear on reg 0x50.
 *
 * Routed to the generated body: every 0x801bcbXX access is a pointer-cell
 * indirection (lw ptr; sb/sw/lbu/lw *ptr) into the CD register file, which the
 * clean port mistranslated as direct DB32 cell stores — the &0x40 done poll
 * then read the pointer value instead of the status byte and spun forever
 * (the boot file-load livelock). br2_call_generated also makes
 * gen_clean_table skip this address. */
uint32_t cd_cmd_issue_b(uint32_t param_1,uint32_t param_2)
{
  return br2_call_generated(0x80197800, param_1, param_2, 0, 0);
}

/* cd_cmd_issue_c — CD command issue B (same protocol, different opcode
 * 0x21020843 / reply 0x11400100). Same pointer-cell indirection -> generated. */
uint32_t cd_cmd_issue_c(uint32_t param_1,uint32_t param_2)
{
  return br2_call_generated(0x80197900, param_1, param_2, 0, 0);
}

/* cd_cmd_retry — CD retry wrapper: forwards arg0 to cd_cmd_bytes,
 * returns 1 (v0). Callers pass a buffer pointer. */
uint32_t cd_cmd_retry(uintptr_t param_1)
{
  cd_cmd_bytes(param_1);
  return 1;
}

/* ---- batch30: file-pump dispatch wrappers ---- */
extern void mdec_pump_b(void);
extern void mdec_pump_a(int32_t);
extern void cd_dispatch_tblc(void);

/* cd_fileop_dispatch — dispatch file op 1 through cd_dispatch_tbl4 table. */
void cd_fileop_dispatch(uint32_t param_1)
{
  cd_dispatch_tbl4(1,param_1);
}

/* cd_fileop_forward — forward to mdec_pump_b. */
void cd_fileop_forward(void)
{
  mdec_pump_b();
}

/* cd_fileop_reset — param_1==0 resets via cd_dispatch_tblc, then mdec_pump_a. */
void cd_fileop_reset(int32_t param_1)
{
  if (param_1 == 0) cd_dispatch_tblc();
  mdec_pump_a(param_1);
}

/* cd_u16_swap_store — u16 swap-store through DAT_801bdda8 pointer. */
uint16_t cd_u16_swap_store(uint16_t param_1)
{
  uint32_t slot = DB32(0x801bdda8);
  uint16_t uVar1 = br2_ld16(slot);
  br2_st16(slot, param_1);
  return uVar1;
}

/* cd_cmd_bytes — CD command issue: writes command/param bytes through
 * the register-pointer cells at 0x801bcb0c/10/14/18 (double
 * indirection: cell -> guest reg addr -> byte). Sequence: 2, a0[0],
 * a0[1], 3, a0[2], a0[3], 0x20. Disasm-verified. */
void cd_cmd_bytes(uintptr_t param_1)
{
  uint8_t *a0 = (uint8_t *)guest_at((uint32_t)param_1);
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
uint32_t cd_xchg_bde20(uint32_t param_1)
{
  uint32_t *p = (uint32_t *)guest_at(0x801bde20);
  uint32_t old = *p;
  *p = param_1;
  return old;
}

/* cd_xchg_bc850 — get-and-set u32 @0x801bc850, returns previous. */
uint32_t cd_xchg_bc850(uint32_t param_1)
{
  uint32_t *p = (uint32_t *)guest_at(0x801bc850);
  uint32_t old = *p;
  *p = param_1;
  return old;
}

/* cd_file_open_b — CD file open: when flag @0x801ec6f8 clear, cd_stamp_to_sector
 * on block @0x801feb00 then cd_file_open(fd+1,arg); returns u32
 * @0x801feb04, else -1. Disasm 0x8019917c-0x801991cc. */
extern void cd_file_open(int32_t,uint8_t *);
int32_t cd_file_open_b(uint32_t param_1)
{
  int32_t v0;
  if (*(uint32_t *)guest_at(0x801ec6f8) != 0) return -1;
  v0 = cd_stamp_to_sector(guest_at(0x801feb00));
  cd_seek_note(3, *(uint32_t *)guest_at(0x801feb00), (uint32_t)v0);
  cd_file_open(v0 + 1,(uint8_t *)guest_at(param_1));
  return *(int32_t *)guest_at(0x801feb04);
}

/* cd_read_param_set — CD read setup: arg -> @0x801feb38, -1 -> @0x801feb28,
 * cd_dispatch_tblc dispatch, cd_cmd_sequence(0x20000001,&block), bios_table_tramp(0).
 * Disasm 0x80199f30-0x80199f74. */
extern void cd_cmd_sequence(uint32_t,uint32_t,uint32_t,uint32_t);
void cd_read_param_set(uint32_t param_1)
{
  *(uint32_t *)guest_at(0x801feb38) = param_1;
  *(uint32_t *)guest_at(0x801feb28) = 0xffffffff;
  cd_dispatch_tblc();
  cd_cmd_sequence(0x20000001,0x801feb28,0,0);
  bios_table_tramp();
}

/* cd_chan_init_wrap — thin wrapper over cd_channel_main_init. Disasm verified. */
extern void cd_channel_main_init(void);
void cd_chan_init_wrap(void)
{
  cd_channel_main_init();
}

/* cd_subsystem_reset — CD subsystem reset: FUN_8018e300, then mode dispatch on
 * *(u32*)0x801bc868 (==1 -> a120c/a11e4 else 961a8/95d78), zero two
 * indirect bytes, FUN_8018e310(0). Disasm 0x80199070-0x801990e8. */
extern void FUN_8018e300(void);
extern void FUN_8018e310(uint32_t);
extern void cd_tbl4_dispatch3(int32_t);
extern uint32_t cd_xchg_fefb4(uint32_t);
void cd_subsystem_reset(void)
{
  uint8_t *p;
  FUN_8018e300();
  if (*(uint32_t *)guest_at(0x801bc868) == 1) {
    cd_tbl4_dispatch3(0);
    cd_xchg_fefb4(0);
  } else {
    cd_cmd_dispatch3(0);
    mc_session_b(0);
  }
  p = (uint8_t *)guest_at(*(uint32_t *)guest_at(0x801b9bd0));
  if (p) p[0] = 0;
  p = (uint8_t *)guest_at(*(uint32_t *)guest_at(0x801b9bdc));
  if (p) p[0] = 0;
  FUN_8018e310(0);
}

/* cd_probe_retry — CD probe retry: up to 5 cd_ready_probe attempts; on first
 * success installs fn-ptr triple @0x801bc848/4c/1b0 (+0 @0x801bc7b4)
 * and returns 1; else printf 0x80125638, returns 0. Disasm-verified. */
extern int32_t cd_ready_probe(void);
int32_t cd_probe_retry(void)
{
  int32_t s0;
  for (s0 = 4; s0 != -1; s0--) {
    if (cd_ready_probe() == 1) {
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
void cd_channel_table_init(uint8_t *param_1,uint8_t *param_2)
{
  static const uint32_t fns[7] = {
    0x801a38b8,0x801a3850,0x801a39bc,0x801a3a78,0x801a3cf4,0x801a3d2c,0x801a39ac };
  uint8_t *s0 = (uint8_t *)guest_at(0x801ff080);
  uint32_t a2, a3;
  int t;
  if (s0 == NULL) return;
  *(uint32_t *)guest_at(0x801c47b8) = 0;
  *(uint32_t *)guest_at(0x801c47cc) = 0;
  cd_handler_triple_install();
  for (t = 0; t < 7; t++)
    *(uint32_t *)guest_at(0x801c4780 + t * 4) = fns[t];
  *(uint32_t *)guest_at(0x801c47b4) = 0x801ff080;
  cd_bzero(0x801ff080,0x1e0);
  *(uint32_t *)(s0 + 0x30) = guest_of(param_1);
  *(uint32_t *)(s0 + 0x120) = guest_of(param_2);
  a2 = 0x801feff0;
  a3 = 0x801ff038;
  for (t = 0; t < 2; t++) {
    uint8_t *b = s0 + t * 0xf0;
    uint8_t *a0 = b + 0x40;
    uint8_t *p = (uint8_t *)guest_at(*(uint32_t *)(a0 - 0x10));
    int i;
    *(uint32_t *)(a0 - 0x34) = 0;
    *(uint32_t *)(a0 - 0x30) = 0x801ff080 + (uint32_t)t * 0xf0;
    if (p) { p[0] = 0xff; p[1] = 0; }
    *(uint32_t *)(a0 - 4) = a2;
    *(uint32_t *)(a0 + 0) = a3;
    for (i = 0; i < 6; i++) b[0x5d + i] = 0xff;
    a2 += 0x23;
    a3 += 0x23;
  }
  cd_handler_pair_install();
  *(uint32_t *)guest_at(0x801c47b8) = 1;
}

/* cd_tbl4_dispatch3 — dispatch wrapper: cd_dispatch_tbl4(3,arg). */
void cd_tbl4_dispatch3(int32_t param_1)
{
  cd_dispatch_tbl4(3,(uint32_t)param_1);
}

/* cd_xchg_fefb4 — get-and-set u32 @0x801fefb4, returns previous. */
uint32_t cd_xchg_fefb4(uint32_t param_1)
{
  uint32_t *p = (uint32_t *)guest_at(0x801fefb4);
  uint32_t old = *p;
  *p = param_1;
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
void cd_cmd_sequence(uint32_t param_1,uint32_t param_2,uint32_t param_3,uint32_t param_4)
{
  /* BIOS kernel-object boundary — run the recompiled guest body. */
  br2_call_generated(0x8019a00c,param_1,param_2,param_3,param_4);
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
  uint32_t *v1, base;
  FUN_8018e300();
  *(uint32_t *)guest_at(0x801c47b8) = 0;
  bios_tramp_c003(2,0x801fefc0);
  bios_tramp_c002(2,0x801fefc0);
  v1 = (uint32_t *)guest_at(*(uint32_t *)guest_at(0x801c47e0));
  if (v1) {
    *v1 = 0xfffffffe;
    v1[1] |= 1;
  }
  bios_kobj_tramp();
  FUN_8018e310(0);
  base = *(uint32_t *)guest_at(0x801c47b4);
  {
    uint32_t fn = *(uint32_t *)guest_at(0x801c4784);
    call_guest(fn,base,0);
    call_guest(fn,base + 0xf0,0);
  }
  {
    uint32_t *z = (uint32_t *)guest_at(0x801fefd0);
    if (z) { z[0] = 0; z[1] = 0; }
  }
  *(uint32_t *)guest_at(0x801c47b8) = 1;
}

/* cd_dir_entry_probe — CD directory entry probe/free: magic-div index into
 * 0x20-stride table @*(u32*)0x801fc6c0, checks tag==4, clears link
 * fields of count entries, stores cursor @0x801fc6ac.
 * Disasm 0x80199270-0x80199314. */
uint32_t cd_dir_entry_probe(int32_t param_1)
{
  int32_t a0 = param_1;
  int32_t sign, hi, idx, count, i;
  uint32_t *base, *rec;
  uint32_t table_guest;
  int64_t prod;
  CDDBG_INC(0x801bcf24);              /* DIAG: probe calls */
  CDDBG_SET(0x801bcf28, (uint32_t)param_1); /* DIAG: probe arg */
  /* param_1 and the table base are GUEST addresses (KSEG0). The index math
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
  if (*(int16_t *)rec != 4) { CDDBG_SET(0x801bce8c,param_1); CDDBG_SET(0x801bce90,idx); CDDBG_SET(0x801bce94,1); CDDBG_SET(0x801bcf2c,idx); return 1; }
  CDDBG_INC(0x801bcf30);              /* DIAG: probe freed entry */
  count = (int16_t)*(uint16_t *)((uint8_t *)rec + 6);
  a0 = 0;   /* MIPS 0x801992D8: move a0,zero — cursor addend baseline */
  if (count > 0) {
    for (i = 0; i < count; i++) {
      uint32_t *r = (uint32_t *)((uint8_t *)base + ((i + idx) << 5));
      *(int16_t *)r = 0;
    }
    a0 = count;   /* free-loop leaves a0 == count (0x801992E4 addiu) */
  }
  CDDBG_SET(0x801bce8c,param_1); CDDBG_SET(0x801bce90,idx); CDDBG_SET(0x801bce94,*(uint32_t *)guest_at(0x801fc6ac)); CDDBG_SET(0x801bce98,(uint32_t)(a0+idx));
  *(uint32_t *)guest_at(0x801fc6ac) = (uint32_t)(a0 + idx);
  return 0;
}

/* cd_xchg_fefb8 — get-and-set u32 @0x801fefb8, returns previous. */
uint32_t cd_xchg_fefb8(uint32_t param_1)
{
  uint32_t *p = (uint32_t *)guest_at(0x801fefb8);
  uint32_t old = *p;
  *p = param_1;
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
int cd_dispatch_tbl8(int param_1,uint32_t param_2)
{
  uint32_t tbl = CDB32G(0x801bdda0);
  uint32_t fn = *(uint32_t *)guest_at(tbl + 8);
  return (int)call_guest(fn,(uint32_t)param_1,param_2);
}

/* Two-level CD register cells: cell @0x801bcbXX holds a guest
 * pointer; the register byte/half/word lives at *cell. */
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
  uint8_t b[4];
  base = DB32G(0x801bcb20);
  if (base == 0) return 0;
  v0 = br2_ld16(base + 0xdc * 2);        /* +0x1b8 */
  if (v0 == 0) {
    v0 = br2_ld16(base + 0xdd * 2);      /* +0x1ba */
    if (v0 == 0) {
      v0 = 0x3fff;
      br2_st16(base + 0xc0 * 2, v0);     /* +0x180 */
      br2_st16(base + 0xc1 * 2, v0);     /* +0x182 */
    }
    v0 = 0x3fff;
  }
  br2_st16(base + 0xd8 * 2, v0);         /* +0x1b0 */
  br2_st16(base + 0xd9 * 2, v0);         /* +0x1b2 */
  br2_st16(base + 0xd5 * 2, 0xc001);     /* +0x1aa */
  b[2] = 0x80; b[0] = 0x80; b[3] = 0; b[1] = 0;
  if (br2_ld8(DB32G(0x801bcb0c))) br2_st8p(0x801bcb0c, 2);
  if (br2_ld8(DB32G(0x801bcb14))) br2_st8p(0x801bcb14, b[0]);
  if (br2_ld8(DB32G(0x801bcb18))) br2_st8p(0x801bcb18, b[1]);
  if (br2_ld8(DB32G(0x801bcb0c))) br2_st8p(0x801bcb0c, 3);
  if (br2_ld8(DB32G(0x801bcb10))) br2_st8p(0x801bcb10, b[2]);
  if (br2_ld8(DB32G(0x801bcb14))) br2_st8p(0x801bcb14, b[3]);
  if (br2_ld8(DB32G(0x801bcb18))) br2_st8p(0x801bcb18, 0x20);
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
    if (cd_cmd_issue(1,0,0,(int)*(uint32_t *)guest_at(0x801bcb0c)) != 0) return -1;
  }
  if (*(uint32_t *)guest_at(0x801bc854) & 0x10) {
    if (cd_cmd_issue(1,0,0,0) != 0) return -1;
  }
  if (cd_cmd_issue(0xa,0,0,0) != 0) return -1;
  if (cd_cmd_issue(0xc,0,0,0) != 0) return -1;
  return cd_result_wait_a(0,0) == 2 ? 0 : -1;
}

/* cd_kobj_init_a — kernel-object init A: crit-in, BIOS svc B0/0x57 ->
 * object ptr, clears 9 u32 at obj+0x62c, bios_tramp_a044, crit-out.
 * Disasm 0x8019a3f0-0x8019a450. */
extern void bios_tramp_a044(void);
extern void br2_guest_ra_set(uint32_t);
void cd_kobj_init_a(void)
{
  /* BIOS svc B0/0x57 kernel-object boundary — run the recompiled guest body. */
  br2_call_generated(0x8019a3f0,0,0,0,0);
}

/* cd_kobj_init_b — kernel-object init B: crit-in, BIOS svc B0/0x57 ->
 * obj, sub = *(u32*)(obj+0x16c); stores sub+0x884 -> @0x801feb58,
 * sub+0x894 -> @0x801feb5c, clears 11 u32 at sub+0x594, bios_tramp_a044.
 * No crit-out (matches guest). Disasm 0x8019a378-0x8019a3e0. */
void cd_kobj_init_b(void)
{
  /* BIOS svc B0/0x57 kernel-object boundary — run the recompiled guest body. */
  br2_call_generated(0x8019a378,0,0,0,0);
}

/* bios_tramp_a044 — BIOS svc A0/0x44 trampoline. */
void bios_tramp_a044(void) { br2_bios_tramp(0xa0,0x44); }

/* cd_dir_table_set — set dir table base/cursor: a0 -> @0x801fc6c0,
 * a1 -> @0x801fc6c4, then cd_dir_reset. Disasm 0x80195800-24. */
extern void cd_dir_reset(void);
void cd_dir_table_set(uint32_t a0,uint32_t a1)
{
  /* DIAG: log every (a0,a1) plus live _DAT_801f4754 to find the fc6c0=0 call. */
  static uint32_t n;
  uint32_t slot = 0x801bcfa0 + (n & 7) * 0x10;
  CDDBG_SET(slot + 0x00, a0);
  CDDBG_SET(slot + 0x04, a1);
  CDDBG_SET(slot + 0x08, *(uint32_t *)guest_at(0x801f4754));
  CDDBG_SET(slot + 0x0c, n);
  n++;
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
    uint32_t sh = *(uint32_t *)guest_at(0x801c0000 - 0x2128);
    base[a0] = (uint16_t)(a1 >> sh);
  }
}

/* cd_reg_window_update — cd_chan_attr_push(); cd_reg_window_u16(1,*(u32*)a0). */
extern void cd_chan_attr_push(uint32_t);
extern void cd_reg_window_u16(int32_t,uint32_t);
void cd_reg_window_update(uint8_t *a0)
{
  cd_chan_attr_push((uint32_t)(uintptr_t)a0);
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
 * @0x801fefc4=0x801a189c, @0x801fefc8=0x801a1834. Disasm verified. */
void cd_handler_pair_install(void)
{
  *(uint32_t *)guest_at(0x801fefc0) = 0;
  *(uint32_t *)guest_at(0x801fefc4) = 0x801a189c;
  *(uint32_t *)guest_at(0x801fefc8) = 0x801a1834;
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
extern void cd_ext_table_init(uint32_t);
extern uint32_t FUN_8019b380(uint32_t,uint32_t,uint32_t,uint32_t);
extern void FUN_8019b390(uint32_t);
void cd_ext_init_once(void)
{
  uint32_t r;
  if (*(uint32_t *)guest_at(0x801bdea0) != 0) return;
  *(uint32_t *)guest_at(0x801bdea0) = 1;
  FUN_8018e300();
  cd_ext_table_init(0x8019b7e0);
  r = FUN_8019b380(0xf0000009,0x20,0x2000,0);
  FUN_8019b390(r);
  *(uint32_t *)guest_at(0x801bde38) = r;
  FUN_8018e310(0);
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

/* cd_mode_set_0e — CD mode/flag set: cd_mode_flag_set(0xe,&byte,0);
 * when bit 0x100 set, clears @0x801fc6f8 and runs 961a8/95d78 pair;
 * always cd_mode_flag_set(0x1b,0,0). Disasm 0x80198f60-0x80198fdc.
 * The generated body builds the Setmode param byte on the GUEST stack
 * (sp+16 after a 32-byte frame) and passes that guest address as param_2
 * to cd_mode_flag_set -> cd_cmd_issue. A clean C `&b` is a host stack
 * pointer that generated cd_cmd_issue reads as a guest address -> out of
 * guest RAM -> delivers mode 0x00 instead of the computed byte -> drive
 * stays in user-data mode (mode_reg&0x20==0), never delivers the per-sector
 * MSF stamp cd_read_step_cb verifies -> "sector error" -> retry livelock.
 * Delegate the whole body so the param byte lands in real guest RAM. */
void cd_mode_set_0e(uint32_t s0)
{
  (void)br2_call_generated(0x80198f60u, s0, 0, 0, 0);
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

/* cd_ext_table_init — dispatch wrapper: cd_dispatch_tbl4(4,a0). */
void cd_ext_table_init(uint32_t a0) { cd_dispatch_tbl4(4,a0); }

/* cd_chan_block_init — indirect dispatch tbl+0x14: call_guest(fn,a0,a1). */
void cd_chan_block_init(int32_t a0,uint32_t a1)
{
  uint32_t tbl = CDB32G(0x801bdda0);
  uint32_t fn = *(uint32_t *)guest_at(tbl + 0x14);
  (void)call_guest(fn,(uint32_t)a0,a1);
}

/* cd_mode_reset_seq — 99fe0(0), 9e410, 8e310 sequence wrapper.
 * Disasm 0x8019e39c-0x8019e3cc. */
extern void cd_mode_apply_wrap(void);
void cd_mode_reset_seq(void)
{
  FUN_8018e300();
  cd_mode_apply_wrap();
  bios_table_tramp();
  FUN_8018e310(0);
}

/* cd_dir_reset — CD dir reset: a1=*0x801fc6c4, clears
 * @0x801fc6ac/a8/a4/69c, cd_dir_clear(0,a1), @0x801fc68c=0,
 * @0x801fc684(u16)=0, @0x801fc680=0. Disasm verified. */
extern void cd_dir_clear(uint32_t,uint32_t);
void cd_dir_reset(void)
{
  uint32_t a1 = *(uint32_t *)guest_at(0x801fc6c4);
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
extern void cd_mode_apply(uint32_t);
extern void FUN_8019e5cc(void);
extern void FUN_8019e4c8(void);
extern void FUN_8019e55c(void);
extern int32_t cd_status_bcc90(void);
void cd_mode_apply_seq(uint32_t a0)
{
  bios_table_tramp();
  FUN_8018e300();
  if (cd_status_bcc90() == 0) a0 = 0;
  cd_mode_apply(a0);
  FUN_8019e5cc();
  FUN_8019e4c8();
  FUN_8019e55c();
  FUN_8018e310(0);
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
  lo = (int32_t)a1 < 1;
  if (lo || (a1 & 2)) {
    base = *(uint32_t *)guest_at(0x801bdeb0);
    CDMMIO16_ST(base,0xc2,*(uint16_t *)(b + 8));      /* +0x184 */
    *(uint16_t *)guest_at(0x801bde58) = *(uint16_t *)(b + 8);
  }
  if (!lo && (a1 & 4)) {
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
int32_t cd_channel_poll(uint32_t *param_1, uint32_t *param_2)
{
  uint32_t base = *(uint32_t *)guest_at(0x801fc6c0);
  uint32_t idx = *(uint32_t *)guest_at(0x801fc6ac);
  uint8_t *e = (uint8_t *)guest_at(base + idx * 0x20);
  CDDBG_INC(0x801bcf14);              /* DIAG: channel_poll calls */
  CDDBG_SET(0x801bcf18, idx);         /* DIAG: poll idx seen */
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
  if (*(uint16_t *)e != 2) { CDDBG_SET(0x801bcf1c, *(uint16_t *)e); return 1; }
  CDDBG_INC(0x801bcf20);              /* DIAG: poll found state2 */
  *(uint16_t *)e = 4;
  *param_1 = base + *(uint32_t *)guest_at(0x801fc6c4) * 0x20 + idx * 0x7e0;
  *param_2 = base + idx * 0x20;
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
int32_t mc_op_result_poll(uint32_t a0)
{
  uint32_t s0 = 0, sp10 = 0, sp14 = 0, r;
  for (;;) {
    while (cd_chan_job_submit(a0) == 0) { br2_irq_poll(); }
    a0 = 0;
    while ((r = cd_job_result(a0,&sp10,&sp14)) == 0) { br2_irq_poll(); a0 = 0; }
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

/* cd_range_table_member — range-table membership: a0 <<= shift@0x801bdee8, scan
 * 8-byte entries @*(u32*)0x801bdf18; 0x80000000 -> skip, 0x40000000 ->
 * return 0; masked>=a0 or a0<masked+field4 -> return 1; else next.
 * Disasm 0x8019c060-0x8019c0e8. */
int32_t cd_range_table_member(uint32_t a0)
{
  uint32_t sh = *(uint32_t *)guest_at(0x801bdee8);
  uint8_t *p = (uint8_t *)guest_at(*(uint32_t *)guest_at(0x801bdf18));
  uint32_t v1;
  if (p == NULL) return 0;
  a0 <<= sh;
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

/* cd_status_shifted_read — status read: if (a0-0x1010) <u 0x7efe8 ->
 * sh = cd_reg_indexed_rw(-1) @0x801bdec8; return sh << shift@0x801bded8;
 * else 0. Disasm 0x8019d470-0x8019d4c8. */
extern int32_t cd_reg_indexed_rw(int32_t,uint32_t);
uint32_t cd_status_shifted_read(uint32_t a0)
{
  if (a0 - 0x1010 < 0x7efe8) {
    int32_t r = cd_reg_indexed_rw(-1,a0);
    uint32_t sh = (uint16_t)r;
    *(uint16_t *)guest_at(0x801bdec8) = (uint16_t)sh;
    sh = *(uint16_t *)guest_at(0x801bdec8);
    return sh << *(uint32_t *)guest_at(0x801bded8);
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
/* cd_seek_complete_poll — delegate the whole body to generated MIPS
 * (0x8019d500). Its a0==1 seek-completion wait loop crosses a basic-block
 * boundary every iteration, so psx_check_interrupts_at runs per pass with a
 * real guest resume PC. That is what lets the CD deferred-interrupt
 * coroutine (cd_ctx_restore -> cd_chan_full_init -> cd_intc_dispatch)
 * resume and DeliverEvent the 0xf0000009 flag TestEvent polls. A clean-C
 * while+br2_irq_poll cannot supply a dispatchable resume PC for the
 * deferred switch, so the coroutine starves and TestEvent spins forever —
 * the same reason cd_file_read_seq/cd_result_wait_a/b were delegated.
 * br2_call_generated also makes gen_clean_table skip 0x8019d500 so
 * generated callers dispatch straight to the generated body. */
int32_t cd_seek_complete_poll(uint32_t a0)
{
  (void)cd_seek_state_machine;
  return (int32_t)br2_call_generated(0x8019d500, a0, 0, 0, 0);
}

/* cd_dir_read — memcard dir read: mc_session(0) session, cmd 0x13
 * fetches BCD bounds into resp[1..2] (s0..s3), header record written,
 * then cmd 0x14 per index s0..s3 writes {resp[1],resp[2],0} entries.
 * Debug prints gated on @0x801cc850. Returns end-ptr (debug<2) or
 * entry count (debug>=2) — guest quirk preserved. Disasm 0x80195854-0x80195a74. */
uint32_t cd_dir_read(uint32_t a0,uint32_t a1)
{
  uint8_t cmd[1];
  uint8_t resp[8];
  uint8_t *out = (uint8_t *)guest_at(a1);
  uint32_t s6, s0, s3, s2, dbg, i;
  uint8_t *s1p;
  (void)a0;
  cmd[0] = 1;
  s6 = mc_session(0);
  if (!cd_cmd_issue_sync(0x13,0,(undefined4)(uintptr_t)resp)) goto fail;
  s0 = ((uint32_t)(resp[1] >> 4)) * 10 + (resp[1] & 0xf);
  s3 = ((uint32_t)(resp[2] >> 4)) * 10 + (resp[2] & 0xf);
  dbg = *(uint32_t *)guest_at(0x801cc850);
  if (dbg >= 2)
    printf_forward((const char *)guest_at(0x801255f8),s0,s3);
  cmd[0] = 0;
  if (!cd_cmd_issue_sync(0x14,(int)(uintptr_t)cmd,(undefined4)(uintptr_t)resp)) goto fail;
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
    if (!cd_cmd_issue_sync(0x14,(int)(uintptr_t)cmd,(undefined4)(uintptr_t)resp)) goto fail;
    s2++;
    s1p[0] = resp[1];
    s1p[2] = 0;
    s1p[1] = resp[2];
    s0++;
    s1p += 4;
    if (s3 < s0) break;
  }
done:
  dbg = *(uint32_t *)guest_at(0x801cc850);
  if (dbg >= 2) {
    int32_t n = (int32_t)s2 - 1;
    if (n >= 0) {
      uint8_t *p = out;
      for (i = 0; i < (uint32_t)n; i++) {
        printf_forward((const char *)guest_at(0x80125608),p[0],p[1]);
        p += 4;
      }
    }
    mc_session(s6);
    return (uint32_t)n;
  }
  mc_session(s6);
  return guest_of(s1p);
fail:
  if (*(uint32_t *)guest_at(0x801cc850) != 0)
    printf_forward((const char *)guest_at(0x80125624));
  mc_session(s6);
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
extern uint32_t FUN_801a0260(void *);
extern uint32_t cd_queue_b_status(void);
extern uint32_t cd_status_map(uint32_t);
extern uint32_t cd_xchg_fec24(uint32_t);
extern uint32_t cd_chan_cmd_burst(uint32_t);
extern void FUN_801a05f0(uint32_t);
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
    if (d != 0) {
      uint32_t rem = a1 % d;              /* divu $zero,$a1,$d */
      if (rem != 0)
        a1 = (a1 + d) & ~CDREG32(0x801bdee0);
    }
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
        a2 = 0x7f;
        if (sv < 0x80) a2 = (sv < 0) ? 0 : lu;
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
        if (sv < 0x80) a2 = (sv < 0) ? 0 : lu;
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
 * (s1==0 => apply all). Entries are 0x10-byte stride in the table
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

      /* +0x20 mask: entry[2] = cd_curve_lookup(byte split of scratch,
       * byte split of p.u16[+0x16]) */
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

      /* +0x800 mask: CDMMIO16(E,4) hi byte fields */
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
      while (*v == 0) { br2_irq_poll(); }
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
    FUN_801a05f0(CDREG32(0x801febf0));
    do { br2_irq_poll(); } while (cd_queue_a_read() == 0);
    r = cd_queue_a_status();
    return (int32_t)cd_status_map(r);
  }
  v0 = cd_queue_b_status();
  if ((v0 & 4) != 0) goto retry;
  return (int32_t)cd_status_map(v0);
}

/* cd_chan_job_pump — channel-job pump: submits blk/rec pairs through the
 * 0x801febc0 job channel until fp+a5 records produced; copies 0x28-byte
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
  extern uint32_t br2_guest_frame(uint32_t);
  extern void br2_guest_frame_free(void);
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
  CDREG32(0x801febcc) |= 1u << (CDREG32(0x801febd0) & 31);  /* sllv */
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
      s0 = FUN_801a0260(rec);
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
void cd_chan_reg_init(int32_t param_1)
{
  uint32_t blk;
  uint32_t i, v1;
  int s0 = param_1;

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
    uint32_t e = blk + i * 0x10;          /* 0x10-byte stride */
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
 * Mode name table @0x801be470 + mode*0x44 (0x44-byte records);
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
 * u16s from guest src a0 into reg block +0x1a8 (0x40-byte chunks),
 * pulses +0x1aa bits 0x10, waits +0x1ae bit 0x400 clear, then waits
 * (+0x1ae & 0x7ff) == saved status. b[] = u16 view of block @0x801bdeb0.
 * Disasm 0x8019b620-0x8019b7dc. */
void cd_reg_stream_push(uint32_t a0,uint32_t a1)
{
  uint32_t b = CDREG32(0x801bdeb0);
  uint32_t s1 = a1;
  uint32_t s2 = a0;
  uint32_t s3 = CDMMIO16(b,0xd7) & 0x7ff;          /* +0x1ae, delay slot of jal */
  CDMMIO16_ST(b,0xd3,CDREG16(0x801bdec8));          /* +0x1a6 <- dec8 */
  cd_delay_3c();
  while (s1 != 0) {
    uint32_t s0 = (s1 < 0x41) ? s1 : 0x40; /* sltiu 0x41 */
    if ((int32_t)s0 > 0) {
      uint32_t v1 = 0;
      do {
        uint16_t w = *(uint16_t *)guest_at(s2);
        s2 += 2;
        v1 += 2;
        CDMMIO16_ST(b,0xd4,w);                      /* +0x1a8 */
      } while (v1 < s0);
    }
    CDMMIO16_ST(b,0xd5,(uint16_t)((CDMMIO16(b,0xd5) & 0xffcf) | 0x10));  /* +0x1aa */
    cd_delay_3c();
    if ((CDMMIO16(b,0xd7) & 0x400) != 0) {          /* +0x1ae bit 0x400 */
      uint32_t v1 = 0;
      for (;;) {
        v1++;
        if (v1 >= 0xf01) {
          printf_forward((const char *)guest_at(0x80125be8),
                       (const char *)guest_at(0x80125c08));
          break;
        }
        if ((CDMMIO16(b,0xd7) & 0x400) == 0) break;
      }
    }
    cd_delay_3c();
    cd_delay_3c();
    s1 -= s0;
  }
  CDMMIO16_ST(b,0xd5,(uint16_t)(CDMMIO16(b,0xd5) & 0xffcf));
  {
    uint32_t want = s3 & 0xffff;
    if ((CDMMIO16(b,0xd7) & 0x7ff) != want) {
      uint32_t v1 = 0;
      for (;;) {
        v1++;
        if (v1 >= 0xf01) {
          printf_forward((const char *)guest_at(0x80125be8),
                       (const char *)guest_at(0x80125c1c));
          break;
        }
        if ((CDMMIO16(b,0xd7) & 0x7ff) == want) break;
      }
    }
  }
}

/* cd_reg_window_op — register-window op dispatch (ops 0..3), returns 0 or
 * -2 on poll timeout. Stack args a1/a2 land at saved slots; op2 shifts
 * a1 right by @0x801bdee8 into dec8/+0x1a6; op3 runs the sector-read
 * setup through MMIO cells 0x801bdeb4/8/c and cmd 0x801bdebc.
 * Disasm 0x8019b944-0x8019bbc0. */
uint32_t cd_reg_window_op(uint32_t a0,uint32_t a1,uint32_t a2)
{
  uint32_t b = CDREG32(0x801bdeb0);
  uint32_t op = a0;
  uint32_t tgt, v1;

  if (op == 1) {
    tgt = CDREG16(0x801bdec8);
    CDREG32(0x801bdf00) = 0;
    v1 = 0;
    while (CDMMIO16(b,0xd3) != tgt) {              /* +0x1a6 */
      v1++;
      if (v1 >= 0xf01) return 0xfffffffe;
    }
    CDMMIO16_ST(b,0xd5,(uint16_t)((CDMMIO16(b,0xd5) & 0xffcf) | 0x20));
    return 0;
  }
  if ((int32_t)op < 2) {
    if (op != 0) return 0;                /* negative op */
    tgt = CDREG16(0x801bdec8);
    CDREG32(0x801bdf00) = a2;
    v1 = 0;
    while (CDMMIO16(b,0xd3) != tgt) {
      v1++;
      if (v1 >= 0xf01) return 0xfffffffe;
    }
    CDMMIO16_ST(b,0xd5,(uint16_t)(CDMMIO16(b,0xd5) | 0x30));  /* ori: no mask clear */
    return 0;
  }
  if (op == 2) {
    uint32_t sh = CDREG32(0x801bdee8);
    uint32_t v = a1 >> (sh & 31);          /* srlv: sa&31 */
    CDREG16(0x801bdec8) = (uint16_t)v;
    CDMMIO16_ST(b,0xd3,(uint16_t)v);
    return 0;
  }
  if (op == 3) {
    /* 0x801bdf00 is the transfer-mode/state cell written by op0/op1 and the
     * strobe path; the generated op3 reads it (lw -0x2100 -> 0x801BDF00) for
     * the want-bits compare, the strobe select, and the ch4 cmd select.
     * 0x801bdec0 is a different cell (DPCR pointer -> 0x1F8010F0); reading it
     * here made want always 0x20 and forced the else-branch, so a zero-length
     * transfer (a2==0) waited on the wrong SPU status bits and never kicked ch4. */
    uint32_t cur = CDREG32(0x801bdf00);
    uint32_t want = (cur == a2) ? 0x30 : 0x20;
    v1 = 0;
    while ((CDMMIO16(b,0xd5) & 0x30) != want) {
      v1++;
      if (v1 >= 0xf01) return 0xfffffffe;
    }
    if (CDREG32(0x801bdf00) == 1) cd_mmio_strobe_22(); else cd_mmio_strobe_20();
    CDREG32(0x801bdf04) = a1;
    {
      uint32_t cnt = (a2 >> 6) + ((a2 & 0x3f) != 0 ? 1u : 0u);
      CDREG32(0x801bdf08) = cnt;
      /* 0x801bdeb4/8/c are pointer cells -> 0x1F8010C0/C4/C8, live DMA-channel
       * registers. The guest_at double-deref lands in the flat 0x1f800xxx
       * scratch mirror, so the DMA setup never reaches hardware (same
       * mistranslation as the GPU pointer cells). br2_st32p derefs the cell
       * then MMIO-writes, matching the generated lw-ptr;sw-val,0(ptr). */
      br2_st32p(0x801bdeb4, a1);
      br2_st32p(0x801bdeb8, (cnt << 16) | 0x10);
      {
        uint32_t cmd = 0x01000201;
        if (CDREG32(0x801bdf00) == 1) cmd = 0x01000200;
        br2_st32p(0x801bdebc, cmd);
      }
    }
    return 0;
  }
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

/* cd_mode_apply_deferred — mode apply with deferred-pause save/restore:
 * mode a0 indexes u32 table @0x801be440 (+a0*4); cd_range_table_member must
 * return 0. s0==0 -> s1=0x10<<sh, s2=0xfff0<<sh; else s1=(-ent)<<sh,
 * s2=ent<<sh (sh=@0x801bdee8). Saves/restores @0x801bdecc (when 1) and
 * @0x801bdee8 pending value. Chunk loop drives cd_reg_window_op ops
 * 2/1/3 then BIOS svc B0/0xa trampoline (FUN_8019cfb0) per 0x400 chunk.
 * NOTE: a1/a2 into the first two b944 calls are caller-carryover regs
 * in the guest (unused by those op paths); approximated as s2/0.
 * Disasm 0x8019ce10-0x8019cfa0. */
int32_t cd_mode_apply_deferred(uint32_t a0)
{
  /* BIOS kernel boundary — run the recompiled guest body. */
  return (int32_t)br2_call_generated(0x8019ce10,a0,0,0,0);
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

/* cd_file_header_copy — 6-byte header copy from payload @0x80125f6c into
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
  /* BIOS kernel boundary — run the recompiled guest body. */
  return (int32_t)br2_call_generated(0x801a0280,(uint32_t)guest_of(a0),(uint32_t)guest_of(a1),0,0);
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

/* cd_chan_cmd_burst — channel command pump: 15 rounds building a 0x20-byte
 * request {0xa0,0,0xffff,0...} at 0x801fec70+i*0x20, copying it to the
 * 0x80-byte work buf @0x801feea0 and calling cd_chan_cmd_pump(a0,i+1,buf);
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
 * @0x801fef60+idx*4, zero 16-byte entry @0x801fef20+idx*0x10.
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
 * a 0x1f8-byte DMA-style command through either the async path
 * cd_copy_u32 or the job path cd_chan_cmd_submit (7 args, last three on
 * the guest stack). Status codes go to *(u32*)0x801bcc88; error
 * retries bump *(u32*)0x801fc6a0 when mode flag 0x801fc6b0 is set.
 * Disasm 0x80199440-0x80199d58. */
extern uint32_t cd_copy_u32(uint32_t,uint32_t,uint32_t,uint32_t);
extern uint32_t cd_chan_cmd_submit(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t,uint32_t,uint32_t);
extern void cd_dir_entry_finalize();

#define CD_ENT16(o)  (*(uint16_t *)guest_at(DB32(0x801feb18) + (o)))

void cd_mc_state_machine(void)
{
  CDDBG_INC(0x801bce88);   /* DIAG: invocation count */
  /* Delegate the whole body to generated MIPS (0x80199440). The clean-C
   * port cannot reserve the guest stack frame the cd_result_wait_b out
   * buffer needs: it passes guest_sp()-0x10 as param_2, but with no
   * sp-=0x40 prologue that address lands inside the callee's own frame
   * (wait_b saves s6/s7 at sp+0x30/0x34), so the 8-byte response copy
   * collides with saved registers and the read-back sees stack garbage
   * (bit4 set -> status 3 livelock). Generated code reserves the real
   * frame, placing the out buffer above the callee frame. Same contract
   * as cd_result_wait_b/cd_file_read_seq. */
  br2_call_generated(0x80199440u,0,0,0,0);
  /* DIAG: histogram the status the generated body left at 0x801bcc88 so we
   * can see which early-return path dominates (4=entry-busy, 8/9=claim-strand). */
  {
    uint32_t st = DB32(0x801bcc88) & 0xf;
    if (st < 16) CDDBG_INC(0x801bcf34 + st * 4);
    CDDBG_SET(0x801bcf78, DB32(0x801bcc88));
    /* DIAG: entry state at the submit cursor + cursor value this call saw. */
    {
      uint32_t cur = DB32(0x801fc6a4);
      uint32_t ent = DB32(0x801fc6c0) + cur * 0x20;
      CDDBG_SET(0x801bcf7c, cur);
      CDDBG_SET(0x801bcf80, *(uint16_t *)guest_at(ent));
      CDDBG_SET(0x801bcf84, ent);
    }
  }
  return;
}

#undef CD_ENT16

/* ======== 0x801a12dc ======== */
/* Device-context status map. ctx = (*(*(u32*)0x801be194))() guest fn
 * ptr via call_guest. Early-out conditions on ctx+0x37/38/39,
 * *(u32*)(ctx+0x30) deref byte, and self-link ctx+0x10==ctx. Returns
 * mapped u8@ctx+0x49: sel==3 or sel==2 -> 1; sel==6 -> 4; else raw.
 * Disasm 0x801a12dc-0x801a13a4. */
uint32_t call_guest(uint32_t, uint32_t, uint32_t);
static uint8_t *ctx_194_get(void)
{
  uint32_t fn = *(uint32_t *)guest_at(0x801be194);
  return (uint8_t *)guest_at(call_guest(fn, 0, 0));
}
int32_t cd_ctx_status_map(int32_t sel)
{
  uint8_t *v1 = ctx_194_get();
  uint8_t a0;
  if (v1[0x37] != 0 || v1[0x38] != 0)
    goto map;
  if (*(uint32_t *)(v1 + 0x10) != guest_of(v1) ||
      v1[0x39] != 0)
    goto map;
  if (*(uint8_t *)*(uint32_t *)(v1 + 0x30) != 0)
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
    if (idx < v1[0xe3])
      return *(uint16_t *)(*(uint8_t **)v1 + idx * 2);
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

/* cd_strcmp — strcmp: NULL/equality guards then signed-byte compare
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
  CDDBG_INC(0x801bcf08);              /* DIAG: finalize (state=2) calls */
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
 * 0x80125b18). Flag byte @*(*(u32*)0x801bcc5c)+2 bit (1<<a0) set when
 * low byte of stack arg6 (a5) == 1 else cleared. Control u32
 * @*(*(u32*)0x801bcc58) |= 1<<(a0*4+3). Then waits bit 0x40 of byte
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
  CDDBG_INC(0x801bcf00);              /* DIAG: chan_cmd_submit calls */
  if (s0 == 3) CDDBG_INC(0x801bcf04); /* DIAG: ch3 (CDROM) arms */
  CDDBG_SET(0x801bcf88, a4);          /* DIAG: last CHCR arg */
  CDDBG_SET(0x801bcf8c, a1);          /* DIAG: last MADR arg */
  CDDBG_SET(0x801bcf90, a0);          /* DIAG: last channel arg */
  CDDBG_SET(0x801bcf94, a3);          /* DIAG: last BCR-low arg */

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
  while ((br2_ld8(fpa) & 0x40) == 0) { br2_irq_poll(); }

  if (s0 == 3) CDDBG_INC(0x801bcf10); /* DIAG: ch3 CHCR kick reached */
  psx_write_word(base + 8, a4);                          /* CHCR: start */
  return psx_read_word(base + 8);                        /* CHCR readback */
}

/* cd_chan_cmd_pump — CD command pump, up to 8 retries. a0 = channel,
 * a1 = sub-command index, a2 = guest ptr to 0x80-byte request whose
 * byte 0x7f receives the XOR checksum of bytes 0..0x7e.
 * Per retry: svc B0/0x50, svc B0/0x4e (must ret 1), poll B0/0x5c
 * (a0 = a0>>4 sra) until bit0, memset work buf, svc B0/0x50 again,
 * svc B0/0x4f (ret!=1 -> printf 0x80125f78, skip poll), optional
 * B0/0x5c poll, then XOR-check response buf byte 0x7f vs request.
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
  return (int32_t)br2_call_generated(0x801a0660u,a0, a1, a2, 0);
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
  uint32_t fn = DB32(0x801be19c);
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
/* ctx callback: per t0 in [0,ctx[0xe9]): count matches of byte t0 in
 * first 6 bytes at ctx[0x20] -> a3; t1 = byte@(ctx[1] + t0*5 + 2)
 * (0->1); second 6-byte scan writes ctx[0x5d+i] = 0xff while a3<t1
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
          /* byte mismatch: still writes? check disasm: sb only on match paths */
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

extern undefined4 cd_strncmp(undefined4,undefined4,undefined4);
extern void FUN_8019a364(void);
extern void FUN_8019a310(void);
extern void FUN_8019a300(void);
extern void FUN_8019a350(void);
extern void FUN_8019e420(void);
extern void FUN_8019e600(void);
extern undefined1 *guest_memcpy(uint32_t *,uint32_t *,int);
int cd_queue_push(uint32_t,uint32_t,uint32_t,uint32_t);
void cd_chan_reset_seq(void);
int cd_crit_c003(void);
void ret_zero_f(void);

/* 0x80195bd0 — BIOS vector trampoline: jr $t2=0xb0, t1=7; a0/a1 fwd. */
uint32_t cd_bios_svc_07(uint32_t a0,uint32_t a1)
{ return bios_svc(0xb0,7,a0,a1); }

/* 0x80199fd0 — BIOS vector trampoline: jr $t2=0xb0, t1=0x16. */
uint32_t cd_bios_svc_16(uint32_t a0,uint32_t a1)
{ return bios_svc(0xb0,0x16,a0,a1); }

/* ======== 0x80197e08 ======== */

int cd_strncmp12_eq(undefined4 param_1,undefined4 param_2)
{
  return cd_strncmp(param_1,param_2,0xc) == 0;
}

/* ======== 0x80198fe4 ======== */

void cd_mc_sm_wrap(void)
{
  cd_mc_state_machine();
}

/* ======== 0x80199fa4 ======== */

void cd_chan_reset_fwd(void)
{
  cd_chan_reset_seq();
}

/* ======== 0x8019e300 ======== */

int32_t cd_reset_chain_a(void)
{
  ret_zero_f();
  return 0;
}

/* ======== 0x80192ef0 ======== */

int cd_queue_push_wrap(uint32_t param_1,uint32_t param_2,uint32_t param_3)
{
  return cd_queue_push(param_1,param_2,0,param_3);
}

/* ======== 0x8019d1f0 ======== */

int cd_tbl8_dispatch9(uint32_t param_1)
{
  return cd_dispatch_tbl8(9,param_1);
}

/* ======== 0x80193ce0 ======== */

int cd_drain_dispatch(void)
{
  return cd_dispatch_tbl4(2,0x801931c4);
}

/* ======== 0x80195b4c ======== */

uint32_t cd_bios_cmd20(void)
{
  return cd_bios_svc_07(0xf0000003,0x20);
}

/* ======== 0x80195b74 ======== */

uint32_t cd_bios_cmd40(void)
{
  return cd_bios_svc_07(0xf0000003,0x40);
}

/* ======== 0x80195b9c ======== */

uint32_t cd_bios_cmd40_b(void)
{
  return cd_bios_svc_07(0xf0000003,0x40);
}

/* ======== 0x80199f7c ======== */

uint32_t cd_bios16_status(void)
{
  cd_bios_svc_16(0,0);
  return ~DB32(0x801feb28);
}

/* ======== 0x8019a15c ======== */

void cd_chan_reset_seq(void)
{
  FUN_8019a364();
  FUN_8019a310();
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
  FUN_8019a300();
  bios_table_tramp();
  FUN_8019a350();
  return 1;
}

/* ======== 0x80191978 ======== */

uint32_t cd_blk5c_copy(uint32_t param_1)
{
  guest_memcpy((uint32_t *)param_1,(uint32_t *)0x801b8330,0x5c);
  return param_1;
}

/* ======== 0x80191ea4 ======== */

uint32_t cd_blk14_copy(uint32_t param_1)
{
  guest_memcpy((uint32_t *)param_1,(uint32_t *)0x801b838c,0x14);
  return param_1;
}

/* ======== 0x801936b0 ======== */

void cd_timeout_arm(void)
{
  DB32(0x801b845c) = cd_timer_read(0xffffffff) + 0xf0;
  DB32(0x801b8460) = 0;
}

/* ======== 0x8019e3d4 ======== */

void ret_zero_f(void)
{
  FUN_8019e420();
  FUN_8019e600();
}

/* ======== 0x8019a710 ======== */

uint32_t cd_ind_dispatch_14(uint32_t param_1)
{
  uint32_t tbl = CDB32G(0x801bdda0);
  uint32_t fn = *(uint32_t *)guest_at(tbl + 0x14);
  return call_guest(fn,4,param_1);
}

/* ======== batch_mass2 ======== */

extern int gpu_coord_pack(int,int);
extern void cd_guest_zero32_c(uint32_t,uint32_t);
extern void cd_guest_zero32_b(uint32_t,uint32_t);
extern void FUN_801a0250(int32_t);
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

uint32_t cd_state9_store(uint32_t param_1)
{
  uint32_t old = DB32G(0x801bdeec);
  if (param_1 != old) {
    DB32(0x801bdeec) = param_1;
    cd_tbl8_dispatch9(param_1);
  }
  return old;
}

/* ======== 0x80191f88 ======== */

int cd_coord_pack_entry(uint32_t param_1,uint32_t param_2)
{
  int iVar1;
  *(uint8_t *)guest_at(param_1 + 3) = 2;
  iVar1 = gpu_coord_pack((int)*(int16_t *)guest_at(param_2),
                       (int)*(int16_t *)guest_at(param_2 + 2));
  *(uint32_t *)guest_at(param_1 + 4) = (uint32_t)iVar1;
  *(uint32_t *)guest_at(param_1 + 8) = 0;
  return iVar1;
}

/* ======== 0x8019d170 ======== */

int32_t cd_reg_rw_d2(uint32_t param_1)
{
  if (param_1 > 0x7ffffff8u) return 0;
  return cd_reg_indexed_rw(0xd2,param_1);
}

/* ======== 0x8019e6f0 ======== */

void cd_block7_init(void)
{
  while (DB32(0x801febe0) != 0) { br2_irq_poll(); }
  cd_chan_block_init(7,0);
  FUN_8019a0ddc();
}

/* ======== 0x8019ef30 ======== */

void cd_pending_release(void)
{
  int32_t v = (int32_t)DB32(0x801febf4);
  if (v >= 0) {
    FUN_801a0250(v);
    DB32(0x801febf4) = 0xffffffff;
  }
}

/* ======== 0x8019746c ======== */

void cd_chan_reset_b(void)
{
  DB32(0x801bc84c) = 0;
  DB32(0x801bc848) = 0;
  DB32(0x801bc858) = 0;
  DB32(0x801bc854) = 0;
  cd_dispatch_tblc();
  cd_dispatch_tbl8(2,0x801979f8);
}

/* ======== 0x8019af30 ======== */

uint32_t cd_init_stage_b(void)
{
  cd_guest_zero32_c(0x801bddf4,8);
  br2_st32p(0x801bddf0, 0);
  cd_dispatch_tbl8(3,0x8019af7c);
  return 0x8019b0fc;
}

/* ======== 0x8019ae10 ======== */

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
static uint32_t cd_gpu_call(uint32_t off,uint32_t a0,uint32_t a1,
                                uint32_t a2,uint32_t a3)
{
  uint32_t *t = (uint32_t *)guest_at(*(uint32_t *)guest_at(0x801b8318));
  return gpu_invoke(t[off >> 2],a0,a1,a2,a3);
}
extern void gpu_rect_bounds_dbg(undefined4,short *);
extern int cd_pump_watchdog(void);
extern void cd_dma_xfer_start(uint32_t,uint32_t,uint32_t);
extern void FUN_8019ad80(uint32_t);

/* ======== 0x80191714 ======== */

void cd_gpu_callpair_a(uint32_t param_1,uint32_t param_2,uint32_t param_3,
                  uint32_t param_4)
{
  uint32_t s1 = DB8(param_1 + 3);
  cd_gpu_call(0x3c,0,param_2,param_3,param_4);
  cd_gpu_call(0x14,param_1 + 4,s1,param_3,param_4);
}

/* ======== 0x80191084 ======== */

uint32_t cd_gpu_defer_set(uint32_t param_1)
{
  uint32_t old = DB32(0x801b832c);
  if (DB8(0x801b8322) >= 2) {
    call_guest(DB32G(0x801b831c),0x80125484,param_1);
  }
  DB32(0x801b832c) = param_1;
  return old;
}

/* ======== 0x80191488 ======== */

void cd_gpu_call_8(uint32_t param_1,uint32_t param_2)
{
  uint32_t obj;
  gpu_rect_bounds_dbg(0x80125510,(short *)(uintptr_t)param_1);
  obj = DB32G(0x801b8318);
  cd_gpu_call(8,*(uint32_t *)guest_at(obj + 0x1c),param_1,8,param_2);
}

/* ======== 0x8019a244 ======== */

void cd_hdr_clear_fixed(void)
{
  uint32_t p = DB32G(0x801bcc94);
  int i;
  *(uint16_t *)guest_at(p + 0xa) = 0;
  for (i = 9; i >= 0; i--) { }
}

/* ======== 0x8019ae68 ======== */

void cd_cb8_dispatch(void)
{
  int i;
  DB32(0x801bdde0) = DB32(0x801bdde0) + 1;
  for (i = 0; i < 8; i++) {
    uint32_t fn = *(uint32_t *)guest_at(0x801bddc0 + i * 4);
    if (fn != 0) call_guest(fn,0,0);
  }
}

/* ======== 0x8019bc48 ======== */

uint32_t cd_win_shift_op(uint32_t param_1,uint32_t param_2)
{
  uint32_t sh = (uint32_t)DB16(0x801bdec8)
              << (DB32(0x801bded8) & 31);
  cd_reg_window_op(2,sh,0);
  cd_reg_window_op(0,sh,0);
  cd_reg_window_op(3,param_1,param_2);
  return param_2;
}

/* ======== 0x8019842c ======== */

int cd_file_load_sync(uint32_t param_1,uint32_t param_2,uint32_t param_3)
{
  uint8_t buf[8];
  cd_file_open((int32_t)param_2,buf);
  cd_mode_flag_set(2,(int)(uintptr_t)buf,0);
  cd_read_setup(param_1,param_3,0x80);
  return cd_read_poll(0,0) == 0;
}

/* ======== 0x8019e730 ======== */

int cd_job_start_e(uint32_t param_1)
{
  if ((int32_t)DB32(0x801febe0) > 0) {
    printf_forward((const char *)guest_at(0x80125ce8));
    return 0;
  }
  DB32(0x801febe0) = 1;
  DB32(0x801febe4) = 0;
  DB32(0x801febe8) = 0;
  DB32(0x801febf0) = param_1;
  cd_job_slot_alloc(0x8019e798,param_1);
  return 1;
}

/* ======== 0x80195c1c ======== */

int cd_init_mode_dispatch(uint32_t param_1)
{
  if (param_1 == 2) {
    cd_chan_reset_b();
    return 1;
  }
  if (cd_subsystem_init() != 0) return 0;
  if (param_1 != 1) return 0;
  if (cd_reg_init_seq() != 0) return 0;
  return 1;
}

/* ======== 0x801915a0 ======== */

uint32_t cd_prim_list_a(uint32_t param_1,uint32_t param_2)
{
  uint32_t s0 = param_1;
  uint32_t s1 = param_2;
  uint32_t a0, v0, v1;
  if (DB8(0x801b8322) >= 2) {
    call_guest(DB32G(0x801b831c),0x80125528,s0);
  }
  s1 = s1 - 1;
  if (s1 != 0) {
    do {
      s1 = s1 - 1;
      a0 = s0 + 4;
      *(uint8_t *)guest_at(s0 + 3) = 0;
      v0 = DB32(s0);
      v1 = a0 & 0xffffff;
      v0 = (v0 & 0xff000000) | v1;
      DB32(s0) = v0;
      if (s1 == 0) break;
      s0 = a0;
    } while (1);
  }
  v0 = s0;
  DB32(0x801b83e0) = (0x801b83cc & 0xffffff) | 0x4000000;
  *(uint32_t *)guest_at(v0) = 0x801b83e0 & 0xffffff;
  return v0;
}

/* ======== 0x80193be4 ======== */

int32_t cd_gpu_call_18(uint32_t param_1)
{
  for (;;) {
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
  cd_gpu_call(0x18,param_1,0,0,0);
  return 0;
}

/* ======== 0x8019cfc0 ======== */

int cd_dma_xfer_mode(uint32_t param_1,uint32_t param_2)
{
  if (param_2 == 5) {
    cd_dma_xfer_start(param_1,0,0x20);
  } else if (param_2 == 6) {
    cd_dma_xfer_start(param_1 + 0x800,0x100,0x20);
  } else {
    cd_dma_xfer_start(param_1,0,0x40);
  }
  return (br2_ld16(DB32G(0x801bdeb0) + 0x1ae) & 0x800) != 0;
}

/* ======== 0x8019aca4 ======== */

uint32_t cd_state_blk_save(void)
{
  uint32_t s0 = 0x801bcd18;
  if (*(uint16_t *)guest_at(s0) != 0) return 0;
  FUN_8019ad80(s0 + 0x38);
  *(uint16_t *)guest_at(s0) = 1;
  br2_st16p(0x801bdda8, *(uint16_t *)guest_at(s0 + 0x32));
  br2_st32p(0x801bddac, *(uint32_t *)guest_at(s0 + 0x34));
  FUN_8018e310(DB32G(0x801bdda8));
  return s0;
}

/* ======== batch_mass4 ======== */

extern void cd_kobj_init_a(void);
extern void cd_kobj_init_b(void);
extern void FUN_8019a2f0(uint32_t,uint32_t,uint32_t,uint32_t);
extern int cd_seek_state_machine(uint32_t);
extern void cd_mmio_strobe_22(void);
extern void FUN_8019ad70(uint32_t);

/* ======== 0x8019a09c ======== */

int cd_subsys_init_full(uint32_t param_1,uint32_t param_2,uint32_t param_3,
                 uint32_t param_4)
{
  cd_kobj_init_a();
  FUN_8018e300();
  cd_kobj_init_b();
  FUN_8018e310(0);
  bios_table_tramp();
  cd_subchan_table_install();
  FUN_8019a2f0(param_1,param_2,param_3,param_4);
  DB32(0x801bcc90) = 1;
  return 1;
}

/* ======== 0x8019ac04 ======== */

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
  FUN_8019ad70(a0);
  *(uint16_t *)guest_at(s0) = 0;
  return s0;
}

/* ======== 0x8019b89c ======== */

void cd_dma_xfer_start(uint32_t param_1,uint32_t param_2,uint32_t param_3)
{
  uint32_t b = DB32G(0x801bdeb0);
  uint32_t v1;
  /* +0x1a6/+0x1aa are channel regs on the MMIO block — CDMMIO not guest_at. */
  CDMMIO16_ST(b,0xd3,(uint16_t)param_2);
  cd_delay_3c();
  b = DB32G(0x801bdeb0);
  v1 = CDMMIO16(b,0xd5);
  CDMMIO16_ST(b,0xd5,(uint16_t)(v1 | 0x30));
  cd_delay_3c();
  cd_mmio_strobe_22();
  br2_st32(DB32G(0x801bdeb4), param_1);
  br2_st32(DB32G(0x801bdeb8), (param_3 << 16) | 0x10);
  DB32(0x801bdea0) = 1;
  br2_st32(DB32G(0x801bdebc), 0x1000200);
}

/* ======== batch_mass5 ======== */

/* ======== 0x80190fd0 ======== */

uint32_t cd_gpu_call_34(uint32_t param_1,uint32_t param_2,uint32_t param_3,
                      uint32_t param_4)
{
  uint32_t s2 = DB8(0x801b8321);
  if (DB8(0x801b8322) >= 2) {
    call_guest(DB32G(0x801b831c),0x80125470,param_1);
  }
  if (param_1 != DB8(0x801b8321)) {
    uint32_t tbl = DB32G(0x801b8318);
    gpu_invoke(*(uint32_t *)guest_at(tbl + 0x34),1,param_2,param_3,param_4);
    DB8(0x801b8321) = (uint8_t)param_1;
    cd_dispatch_tbl4(2,0);
  }
  return s2;
}

/* ======== 0x801980ec ======== */

int cd_file_rec_find(uint32_t param_1,uint32_t param_2)
{
  uint32_t i, off = 0;
  uint32_t s2 = 0x801fcd0c;
  for (i = 0; i < 0x80; i++) {
    uint32_t v0 = *(uint32_t *)guest_at(0x801fcd04 + off);
    if (v0 == 0) return -1;
    if (v0 == param_1) {
      if (cd_strcmp((const void *)(uintptr_t)param_2,
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

int32_t cd_gpu_rect_draw(uint32_t param_1,uint32_t param_2,uint32_t param_3)
{
  int32_t v0;
  uint32_t tbl;
  gpu_rect_bounds_dbg(0x8012551c,(short *)(uintptr_t)param_1);
  v0 = *(int16_t *)guest_at(param_1 + 4);
  if (v0 == 0) return -1;
  v0 = *(int16_t *)guest_at(param_1 + 6);
  if (v0 == 0) return -1;
  *(uint32_t *)guest_at(0x801b83c0) = DB32(param_1);
  *(uint32_t *)guest_at(0x801b83c4) = (param_3 << 16) | (param_2 & 0xffff);
  *(uint32_t *)guest_at(0x801b83c8) = DB32(param_1 + 4);
  tbl = DB32G(0x801b8318);
  gpu_invoke(*(uint32_t *)guest_at(tbl + 8),
                 *(uint32_t *)guest_at(tbl + 0x18),0x801b83b8,0x14,0);
  return (int32_t)DB32(param_1 + 4);
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

int cd_submit5(uint32_t param_1,uint32_t param_2,uint32_t param_3)
{
  return cd_submit_check(param_1,param_2,param_3,5,0x8019f02c);
}

/* ======== 0x8019f168 ======== */

int cd_submit6(uint32_t param_1,uint32_t param_2,uint32_t param_3)
{
  return cd_submit_check(param_1,param_2,param_3,6,0x8019f220);
}

/* ======== batch_mass7 ======== */

extern void gpu_prim_pkt_build(int,ushort *);
extern void cd_guest_zero32_a(uint32_t,uint32_t);
extern int cd_ctx_save(uint32_t);
extern void FUN_8019ad48(uint32_t);
extern void cd_intc_dispatch(void);

/* shared CD wait: poll **0x801b8434 bit23 / **0x801b8428 bit22 with
 * cd_pump_watchdog pump; -1 on pump failure. (0x80193be4/938c8/939b4) */
static int32_t cd_wait_stat(void)
{
  for (;;) {
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

void cd_response_pump(void)
{
  uint32_t s2 = DB8P(0x801bcb0c) & 3;
  uint32_t s0;
  DB32(0x801bce04) = DB32(0x801bce04) + 1;   /* dbg: pump invocations */
  for (;;) {
    s0 = cd_response_process();
    if (s0 == 0) break;
    if ((s0 & 4) != 0 && DB32G(0x801bc84c) != 0) {
      call_guest(DB32G(0x801bc84c),DB8(0x801bcb25),0x801fc6d0);
    }
    if ((s0 & 2) != 0 && DB32G(0x801bc848) != 0) {
      call_guest(DB32G(0x801bc848),DB8(0x801bcb24),0x801fc6c8);
    }
  }
  br2_st8p(0x801bcb0c, (uint8_t)s2);
}

/* ======== 0x80192634 ======== */

int cd_cmd_queue_load(uint32_t param_1,uint32_t param_2)
{
  /* IRQ/coroutine-boundary function: clean-C body cannot supply the
   * live guest register file the deferred CD coroutine needs, and its
   * MMIO busy-waits lack per-block interrupt checks. Delegate to the
   * generated MIPS (behavioral oracle) — same contract as the other
   * read-path functions already delegated. */
  return (int)br2_call_generated(0x80192634u,param_1, param_2, 0, 0);
}

/* ======== 0x801938c8 ======== */

int32_t cd_gpu_call_20(uint32_t param_1,uint32_t param_2)
{
  int32_t r;
  gpu_rect_bounds_dbg(0x801255ec,(short *)(uintptr_t)param_1);
  cd_timeout_arm();
  r = cd_wait_stat();
  if (r != 0) return r;
  cd_dispatch_tbl4(2,0x80193ce0);
  {
    uint32_t tbl = DB32G(0x801b8318);
    gpu_invoke(*(uint32_t *)guest_at(tbl + 0x20),param_1,param_2,0,0);
  }
  return 0;
}

/* ======== 0x801939b4 ======== */

int32_t cd_gpu_call_1c(uint32_t param_1,uint32_t param_2)
{
  int32_t r;
  gpu_rect_bounds_dbg(0x80125510,(short *)(uintptr_t)param_1);
  cd_timeout_arm();
  r = cd_wait_stat();
  if (r != 0) return r;
  cd_dispatch_tbl4(2,0x80193ce0);
  {
    uint32_t tbl = DB32G(0x801b8318);
    gpu_invoke(*(uint32_t *)guest_at(tbl + 0x1c),param_1,param_2,0,0);
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
  return br2_call_generated(0x8019a814, 0, 0, 0, 0);
}

/* ======== 0x801918a0 ======== */

void cd_prim_emit_a(uint32_t param_1,uint32_t param_2)
{
  uint32_t s0;
  uint32_t tbl;
  if (DB8(0x801b8322) >= 2) {
    call_guest(DB32G(0x801b831c),0x80125584,param_1);
  }
  s0 = param_2 + 0x1c;
  gpu_prim_pkt_build((int)s0,(ushort *)(uintptr_t)param_2);
  *(uint32_t *)guest_at(param_2 + 0x1c) =
      (DB32(param_2 + 0x1c) & 0xff000000) | (param_1 & 0xffffff);
  tbl = DB32G(0x801b8318);
  gpu_invoke(*(uint32_t *)guest_at(tbl + 8),
                 *(uint32_t *)guest_at(tbl + 0x18),s0,0x40,0);
  guest_memcpy((uint32_t *)0x801b8330,(uint32_t *)param_2,0x5c);
}

/* ======== batch_mass8 ======== */

extern void cd_queue_drain(void);

/* ======== 0x80193574 ======== */

int cd_queue_poll(uint32_t param_1)
{
  if (param_1 == 0) {
    cd_timeout_arm();
    while (DB32(0x801b8448) != DB32(0x801b844c)) {
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

/* ======== 0x8019d030 ======== */

int cd_reg_bit40_ctrl(uint32_t param_1)
{
  int v0 = (int)param_1;
  if (param_1 == 0 || param_1 == 3) {
    uint32_t b = DB32G(0x801bdeb0);
    int i;
    CDMMIO16_ST(b,0xd5,(uint16_t)(CDMMIO16(b,0xd5) & 0xffbf));
    if ((CDMMIO16(b,0xd5) & 0x40) != 0) {
      for (i = 0; i < 0xf01; i++) {
        b = DB32G(0x801bdeb0);
        if ((CDMMIO16(b,0xd5) & 0x40) == 0) break;
      }
      if (i >= 0xf01) {
        printf_forward((const char *)guest_at(0x80125c38),
                     (const char *)guest_at(0x80125c48));
        return -1;
      }
    }
    v0 = 1;
  }
  if (param_1 == 1 || param_1 == 3) {
    uint32_t b = DB32G(0x801bdeb0);
    int i;
    CDMMIO16_ST(b,0xd5,(uint16_t)(CDMMIO16(b,0xd5) | 0x40));
    if ((CDMMIO16(b,0xd5) & 0x40) == 0) {
      for (i = 0; i < 0xf01; i++) {
        b = DB32G(0x801bdeb0);
        if ((CDMMIO16(b,0xd5) & 0x40) != 0) break;
      }
      if (i >= 0xf01) {
        printf_forward((const char *)guest_at(0x80125c38),
                     (const char *)guest_at(0x80125c58));
        return -1;
      }
    }
    v0 = (int)param_1;
  }
  return v0;
}

/* ======== batch_mass9 ======== */

extern uint32_t FUN_801a0230(uint32_t,uint32_t,uint32_t);
extern uint32_t FUN_801a0240(uint32_t,uint32_t,uint32_t);
extern int32_t FUN_801a0220(uint32_t,uint32_t,uint32_t);
extern void cd_svc_dual(uint32_t);
extern uint32_t cd_queue_b_read(void);
extern int32_t bios_tramp_b035(int32_t,const void *,int32_t);

/* ======== 0x8019f02c ======== */

int cd_job_sm5(uint32_t param_1)
{
  uint32_t *s1 = (uint32_t *)guest_at(param_1);
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
        want = FUN_801a0230(s0[0],s0[1],0);
        if (want == s0[1]) break;
      }
      cd_pending_args_pump();
      for (;;) {
        if (FUN_801a0240(s0[0],s0[3],s0[2]) == 0) break;
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

int cd_job_sm6(uint32_t param_1)
{
  uint32_t *s1 = (uint32_t *)guest_at(param_1);
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
        want = FUN_801a0230(s0[0],s0[1],0);
        if (want == s0[1]) break;
      }
      cd_pending_args_pump();
      for (;;) {
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

int cd_job_submit5(uint32_t param_1)
{
  uint32_t *s1 = (uint32_t *)guest_at(param_1);
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
      int32_t r = FUN_801a0220(0x801fec04,0x8001,0);
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
    FUN_801a0250(s0[4]);
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

int cd_job_submit6(uint32_t param_1)
{
  uint32_t *s1 = (uint32_t *)guest_at(param_1);
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
      int32_t r = FUN_801a0220(0x801fec04,0x8001,0);
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
    FUN_801a0250(s0[4]);
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

uint32_t cd_vcall_38(uint32_t param_1,uint32_t param_2)
{
  uint32_t fn = *(uint32_t *)guest_at(param_1 + 0x38);
  return call_guest(fn,param_1,param_2) >> 31;
}

/* ======== 0x8019a688 ======== */

uint32_t cd_vcall_c(uint32_t param_1,uint32_t param_2)
{
  uint32_t fn = *(uint32_t *)guest_at(param_1 + 0xc);
  return call_guest(fn,param_1,param_2);
}

/* ======== 0x8019a6b8 ======== */

uint32_t cd_vcall_8(uint32_t param_1,uint32_t param_2)
{
  uint32_t fn = *(uint32_t *)guest_at(param_1 + 8);
  return call_guest(fn,param_1,param_2);
}

/* ======== 0x8019a6e8 ======== */

uint32_t cd_vcall_4(uint32_t param_1,uint32_t param_2)
{
  uint32_t fn = *(uint32_t *)guest_at(param_1 + 4);
  return call_guest(fn,param_1,param_2);
}

/* ======== 0x8019a74c ======== */

uint32_t cd_vcall_14(uint32_t param_1,uint32_t param_2)
{
  uint32_t fn = *(uint32_t *)guest_at(param_1 + 0x14);
  return call_guest(fn,param_1,param_2);
}

/* ======== 0x8019a77c ======== */

uint32_t cd_vcall_10(uint32_t param_1,uint32_t param_2)
{
  uint32_t fn = *(uint32_t *)guest_at(param_1 + 0x10);
  return call_guest(fn,param_1,param_2);
}

/* ======== 0x8019a7ac ======== */

uint32_t cd_vcall_18(uint32_t param_1,uint32_t param_2)
{
  uint32_t fn = *(uint32_t *)guest_at(param_1 + 0x18);
  return call_guest(fn,param_1,param_2);
}

/* ======== 0x8019799c ======== */

int cd_mmio_bit_wait(uint32_t param_1,uint32_t param_2)
{
  volatile uint8_t *p = (volatile uint8_t *)guest_at(param_2);
  while ((*p & 0x40) == 0) { br2_irq_poll(); }
  br2_st32(DB32G(0x801bcb50), 0x11400100);
  (void)br2_ld32(DB32G(0x801bcb50));
  return 0;
}

/* ======== 0x80199184 ======== */

int cd_file_open_seq(uint32_t param_1,uint32_t param_2)
{
  if (param_2 == 0) {
    int32_t r = cd_stamp_to_sector(guest_at(0x801fcb00));
    cd_file_open(r + 1,(uint8_t *)param_1);
    return (int)*(int32_t *)guest_at(0x801feb04);
  }
  return -1;
}

/* ======== 0x80191184 ======== */

int cd_gpu_call_3c(uint32_t param_1,uint32_t param_2)
{
  if (param_2 >= 2) {
    uint32_t fn = DB32G(0x801b831c);
    call_guest(fn,0x801254b4,param_1);
  }
  {
    uint32_t tbl = DB32G(0x801b8318);
    uint32_t fn = *(uint32_t *)guest_at(tbl + 0x3c);
    return (int)call_guest(fn,param_1,0);
  }
}

/* ======== 0x8019a24c ======== */

void cd_hdr_clear(uint32_t param_1)
{
  volatile int i;
  *(uint16_t *)guest_at(param_1 + 0xa) = 0;
  for (i = 9; i != -1; i--) {}
}

/* ======== batch_mass11 ======== */

/* ======== 0x80191778 ======== */

void cd_gpu_call_8b(uint32_t param_1,uint32_t param_2)
{
  if (param_2 >= 2) {
    uint32_t fn = DB32G(0x801b831c);
    call_guest(fn,0x80125558,param_1);
  }
  {
    uint32_t tbl = DB32G(0x801b8318);
    uint32_t f2 = *(uint32_t *)guest_at(tbl + 8);
    uint32_t a0v = *(uint32_t *)guest_at(tbl + 0x18);
    call_guest(f2,a0v,param_1);   /* a2/a3 zero in guest */
  }
}

/* ======== 0x8019b310 ======== */

void cd_dma_chan_init(uint32_t param_1)
{
  if (param_1 != 0) return;
  DB32(0x801bdea0) = 1;
  FUN_8018e300();
  cd_ext_table_init(0x8019b7e0);
  {
    uint32_t r = FUN_8019b380(0xf0000009,0x20,0x2000,0);
    DB32(0x801bde38) = r;
    FUN_8019b390(r);
  }
  FUN_8018e310(0);
}

/* ======== 0x80199100 ======== */

void cd_dir_entry_store(uint32_t param_1,uint32_t param_2)
{
  uint8_t *e = (uint8_t *)guest_at(param_2 + ((uint32_t)param_1 << 5));
  uint32_t t;
  uint32_t fn;
  CDDBG_INC(0x801bcf0c);              /* DIAG: store (state=2) calls */
  *(uint16_t *)e = 2;
  memcpy(&t,e + 0x1c,4);                     /* lwl/lwr unaligned u32 */
  *(uint32_t *)guest_at(0x801feb00) = t;
  *(uint32_t *)guest_at(0x801feb04) = *(uint32_t *)(e + 8);
  feb00_note(2, t);
  DB32(0x801bc6a8) = DB32(0x801bc6a4);
  fn = DB32G(0x801feb10);
  if (fn != 0) call_guest(fn,fn,0);        /* jalr $a0: a0 = fn ptr */
  DB32(0x801bc69c) = 0;
}

/* ======== 0x8019d508 ======== */

extern int32_t cd_seek_state_machine(uint32_t);

int32_t cd_seek_step(uint32_t param_1,uint32_t param_2)
{
  /* IRQ-poll spin on TestEvent(0x801bde38) — DeliverEvent needs the deferred
   * CD coroutine resumed on IRQ, which a clean-C spin cannot drive. Delegate
   * to generated (same contract as cd_seek_complete_poll 0x8019d500). */
  return (int32_t)br2_call_generated(0x8019d508u,param_1,param_2,0,0);
}

/* ======== 0x8019b8a4 ======== */

extern void cd_mmio_strobe_22(void);

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

void cd_gpu_call_2c(uint32_t param_1,uint32_t param_2,uint32_t param_3)
{
  if (param_3 >= 2) {
    uint32_t fn = DB32G(0x801b831c);
    call_guest(fn,0x80125540,param_1);   /* a2 = param_2 in guest */
  }
  {
    uint32_t tbl = DB32G(0x801b8318);
    uint32_t f2 = *(uint32_t *)guest_at(tbl + 0x2c);
    call_guest(f2,param_1,param_2);
  }
  DB32(0x801b83e0) = (0x801b83cc & 0x00ffffff) | 0x400000;
  *(uint32_t *)guest_at(param_1) = 0x801b83e0 & 0x00ffffff;
}

/* ======== 0x8019b7e8 ======== */

void cd_dma_chan_reset(uint32_t param_1)
{
  int i;
  if (param_1 == 0) cd_delay_3c();
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
  if (DB32G(0x801bdee8) != 0) {
    call_guest(DB32G(0x801bdee8),0xf0000000,0);
  } else {
    (void)cd_bios_svc_07(0xf0000009,0x20);
  }
}

/* ======== 0x801915a8 ======== */

void cd_prim_list_b(uint32_t param_1,uint32_t param_2,uint32_t param_3)
{
  uint32_t s0 = param_1;
  int32_t s1;
  if (param_3 >= 2) {
    uint32_t fn = DB32G(0x801b831c);
    call_guest(fn,0x80125528,param_1);   /* a2 = param_2 in guest */
  }
  s1 = (int32_t)param_2 - 1;
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

uint32_t cd_chan_slot_set(uint32_t param_1,uint32_t param_2)
{
  uint32_t base = 0x801bcd1c;
  uint32_t *slot = (uint32_t *)guest_at(base + (param_1 << 2));
  uint32_t s4 = *slot;
  uint32_t s3;
  if (param_2 == s4) return s4;
  if (*(uint16_t *)guest_at(base - 4) == 0) return s4;
  uint32_t imask = DB32G(0x801bdda8);
  s3 = br2_ld16(imask);
  br2_st16(imask, 0);
  if (param_2 != 0) {
    uint32_t bit = 1u << param_1;
    uint16_t *f48 = (uint16_t *)guest_at(base - 4 + 0x30);
    *slot = param_2;
    s3 |= bit;
    *f48 = (uint16_t)(*f48 | bit);
  } else {
    uint32_t mask = ~(1u << param_1);
    uint16_t *f48 = (uint16_t *)guest_at(base + 0x2c);
    *slot = 0;
    s3 &= mask;
    *f48 = (uint16_t)(*f48 & mask);
  }
  if (param_1 == 0) {
    uint32_t s0 = param_2 < 1;
    bios_table_tramp();
    /* bios_kobj_tramp is a no-arg trampoline that forwards live cpu->gpr
     * a0-a3 to svc C0/0x0a; a direct C call drops the intended (idx,flag)
     * args, so route through call_guest to load them into gpr first. */
    call_guest(0x8019a670u,3,s0);
  }
  if (param_1 == 4) call_guest(0x8019a670u,0,param_2 < 1);
  if (param_1 == 5) call_guest(0x8019a670u,1,param_2 < 1);
  if (param_1 == 6) call_guest(0x8019a670u,2,param_2 < 1);
  br2_st16(imask, (uint16_t)s3);
  return s4;
}

/* ======== batch_mass13 ======== */

/* ======== 0x8019af84 ======== */

void cd_cb7_run(uint32_t param_1)
{
  uint32_t s3 = 0x00ffffff;
  /* Channel mask comes from DICR via the fixed pointer cell @0x801BDDF0
   * (-> 0x1F8010F4), NOT from a0. The generated body (0x8019AF7C) loads
   * *(0x801BDDF0) then dereferences it; a0 is an INTC-dispatch leftover and
   * is never read. param_1 kept for signature parity only. */
  (void)param_1;
  uint32_t s1 = (br2_ld32(DB32G(0x801bddf0)) >> 24) & 0x7f;
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
         * dereference *tbl, NOT tbl[s0] — the latter double-indexes to
         * base+s0*8 and reads the wrong slot (ch4 -> 0x801BDE14 MMIO junk
         * instead of the 0x801BDE04 handler), so DeliverEvent never fires. */
        uint32_t h = *tbl;
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

int cd_file_open_wait(uint32_t param_1,uint32_t param_2,uint32_t param_3)
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
  cd_file_header_copy(param_1,s0);
  cd_strcat_guard(s0,param_2);
  *(uint32_t *)guest_at(0x801febf0) = param_1;
  for (;;) {
    v0 = FUN_801a0220(guest_of(s0),1,0);
    if (v0 >= 0) {
      FUN_801a0250((uint32_t)v0);
      cd_pending_args_pump();
      *q = FUN_801a0220(guest_of(s0),param_3 | 0x8000,0);
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
      *(uint32_t *)guest_at(0x801febf0) = param_1;
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

int cd_gpu_call_18_b(uint32_t param_1,uint32_t param_2,uint32_t param_3)
{
  uint8_t *s0 = (uint8_t *)guest_at(param_1);
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
  a0p[1] = ((uint32_t)param_3 << 16) | (param_2 & 0xffff);
  a0p[0] = *(uint32_t *)s0;
  a0p[2] = *(uint32_t *)(s0 + 4);
  fn = br2_ld32(DB32G(0x801b8318) + 0x18);
  call_guest(fn,0x801b83b8,0);
  return 0;
}

/* ======== batch_mass15 ======== */

/* ======== 0x8019ae70 ======== */

void cd_cb8_run_param(uint32_t param_1)
{
  uint32_t *tbl = (uint32_t *)guest_at(0x801bddc0);
  int i;
  DB32(0x801bdde0) = param_1 + 1;
  for (i = 0; i < 8; i++) {
    uint32_t f = tbl[i];
    if (f != 0) call_guest(f,param_1,0);
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
 * bytes. NULL guards -> 0/-1/1; signed-byte diff on mismatch; the
 * count bound is a2-1 (compares at most a2 bytes). Disasm
 * 0x80198510-0x8019858c. */
undefined4 cd_strncmp(undefined4 param_1,undefined4 param_2,undefined4 param_3)
{
  const int8_t *a;
  const int8_t *b;
  int32_t n;
  int8_t ca, cb;
  /* guest tests the raw args; guest_at(0) is non-NULL, so check
   * param_1/param_2 (a1 arrives as a u32-held host ptr) */
  if (param_1 == 0) {
    if (param_2 == 0) return 0;
    return (undefined4)-1;
  }
  if (param_2 == 0) return 1;
  a = (const int8_t *)guest_at(param_1);
  b = (const int8_t *)(uintptr_t)param_2;
  n = (int32_t)param_3 - 1;
  if (n < 0) return 0;
  for (;;) {
    ca = *a;
    cb = *b;
    b++;
    if (ca != cb) {
      if (n < 0) return 0;
      return (undefined4)(int32_t)((int32_t)ca - (int32_t)cb);
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

void cd_guest_zero32_b(uint32_t a0,uint32_t a1)
{
  uint32_t *p = (uint32_t *)guest_at(a0);
  uint32_t i;
  for (i = 0; i < a1; i++) p[i] = 0;
}

void cd_guest_zero32_c(uint32_t a0,uint32_t a1)
{
  uint32_t *p = (uint32_t *)guest_at(a0);
  uint32_t i;
  for (i = 0; i < a1; i++) p[i] = 0;
}

/* cd_state_store — store a0 into 0x801beee8 only when different.
 * Disasm 0x8019d5c0-0x8019d5e0. */
void cd_state_store(uint32_t a0)
{
  if (CDREG32(0x801beee8) != a0) CDREG32(0x801beee8) = a0;
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
 * bit 0x100000 set -> 1; empty queue -> epilogue only. Per entry:
 * reschedule via cd_dispatch_tbl4(2,0) when one slot remains and
 * 0x801b832c==0, busy-wait *0x801b8428 & 0x400000, dispatch
 * fn(data,arg), advance tail; loop while queue non-empty and status
 * bit clear. Epilogue restores cd_u16_swap_store save @0x801b8454, fires
 * pending cb @0x801b832c once (clearing flag @0x801b8328), returns
 * (head-tail)&0x3f (callers ignore it; declared void above). */
void cd_queue_drain(void)
{
  if ((br2_ld32(DB32G(0x801b8434)) & 0x100000) != 0)
    return;
  DB32(0x801b8454) = cd_u16_swap_store(0);
  if (DB32(0x801b8448) != DB32(0x801b844c)
      && (br2_ld32(DB32G(0x801b8434)) & 0x100000) == 0) {
    for (;;) {
      uint32_t tail = DB32(0x801b844c);
      uint32_t *e;
      uint32_t fn, dp, arg;
      if (((tail + 1) & 0x3f) == DB32(0x801b8448) && DB32(0x801b832c) == 0)
        cd_dispatch_tbl4(2,0);
      while ((br2_ld32(DB32G(0x801b8428)) & 0x04000000) == 0) { br2_irq_poll(); }
      e = (uint32_t *)guest_at(0x801fae70 + tail * 96);
      fn = e[0]; dp = e[1]; arg = e[2];
      call_guest(fn,dp,arg);          /* jalr fn: a0=data a1=arg */
      DB32(0x801b844c) = (tail + 1) & 0x3f;
      if (DB32(0x801b8448) == DB32(0x801b844c)) break;
      if ((br2_ld32(DB32G(0x801b8434)) & 0x100000) != 0) break;
    }
  }
  cd_u16_swap_store((uint16_t)DB32(0x801b8454));
  if (DB32(0x801b8448) == DB32(0x801b844c)
      && (br2_ld32(DB32G(0x801b8434)) & 0x100000) == 0
      && DB32(0x801b8328) != 0) {
    uint32_t cb = DB32(0x801b832c);
    if (cb != 0) {
      DB32(0x801b8328) = 0;
      call_guest(cb,cb,0);            /* jalr a0: a0 = cb ptr */
    }
  }
  /* guest returns (head-tail)&0x3f; all callers ignore it. */
}

/* cd_queue_push — CD command-queue push (0x80192f14-0x801931c0).
 * a0 = fn ptr, a1 = data ptr (guest addr), a2 = payload byte count
 * (copied as a2/4 u32 words into entry+0xc), a3 = arg. Preamble
 * cd_timeout_arm; full queue ((head+1)&0x3f==tail) -> cd_pump_watchdog
 * pump + cd_queue_drain drain, -1 on pump failure. Saves hw reg via
 * cd_u16_swap_store(0) -> 0x801b8450, sets flag @0x801b8328. Direct-call
 * fast path when byte @0x801b8321==0, or queue empty + status bit
 * 0x100000 clear + @0x801b832c==0: busy-wait *0x801b8428 & 0x400000,
 * fn(data,arg), restore reg, return 0. Otherwise enqueue entry,
 * head=(head+1)&0x3f, restore reg, cd_queue_drain(), return depth. */
int cd_queue_push(uint32_t a0_fn,uint32_t a1_data,uint32_t a2_count,
                 uint32_t a3_arg)
{
  uint32_t head, tail, saved;
  int direct;
  cd_timeout_arm();
  for (;;) {
    head = DB32(0x801b8448);
    tail = DB32(0x801b844c);
    if (((head + 1) & 0x3f) != tail) break;
    if (cd_pump_watchdog() != 0) return -1;
    cd_queue_drain();
  }
  saved = cd_u16_swap_store(0);
  DB32(0x801b8450) = saved;
  DB32(0x801b8328) = 1;
  direct = (*(uint8_t *)guest_at(0x801b8321) == 0);
  if (!direct) {
    head = DB32(0x801b8448);
    tail = DB32(0x801b844c);
    direct = (head == tail
              && (br2_ld32(DB32G(0x801b8434)) & 0x100000) == 0
              && DB32(0x801b832c) == 0);
  }
  if (direct) {
    while ((br2_ld32(DB32G(0x801b8428)) & 0x04000000) == 0) { br2_irq_poll(); }
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
 * A clean C port has no guest register file (host stack is not cpu->gpr),
 * so delegate to the recompiled body which performs the real sw sequence.
 * br2_call_generated in the body also makes gen_clean_table skip this
 * address, so jal 0x8019ad90 runs generated code directly. */
int cd_ctx_save(uint32_t a0)
{
  return (int)br2_call_generated(0x8019ad90, a0, 0, 0, 0);
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
void cd_intc_dispatch(void)
{
  /* scratch debug counter @0x801bce00 — bumped on every clean-thunk entry so
   * read_ram can confirm whether the CD IRQ dispatcher is ever invoked. */
  DB32(0x801bce00) = DB32(0x801bce00) + 1;
  /* BIOS kernel boundary — run the recompiled guest body. */
  br2_call_generated(0x8019a8ec,0, 0, 0, 0);
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
  extern uint32_t bios_svc6(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
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
void cd_hw_chan_init(void)
{
  uint32_t r;
  FUN_8018e300();
  r = FUN_8019b380(0xf4000001,0x4,0x1000,0x801a0b10);
  DB32(0x8020ef70) = r;
  r = FUN_8019b380(0xf4000001,0x8000,0x1000,0x801a0b24);
  DB32(0x8020ef74) = r;
  r = FUN_8019b380(0xf4000001,0x100,0x1000,0x801a0b38);
  DB32(0x8020ef78) = r;
  r = FUN_8019b380(0xf4000001,0x2000,0x1000,0x801a0b4c);
  DB32(0x8020ef7c) = r;
  r = FUN_8019b380(0xf0000011,0x4,0x1000,0x801a0b60);
  DB32(0x8020ef80) = r;
  r = FUN_8019b380(0xf0000011,0x8000,0x1000,0x801a0b74);
  DB32(0x8020ef84) = r;
  r = FUN_8019b380(0xf0000011,0x100,0x1000,0x801a0b88);
  DB32(0x8020ef88) = r;
  r = FUN_8019b380(0xf0000011,0x2000,0x1000,0x801a0b9c);
  DB32(0x8020ef8c) = r;
  FUN_8019b390(DB32(0x8020ef70));
  FUN_8019b390(DB32(0x8020ef74));
  FUN_8019b390(DB32(0x8020ef78));
  FUN_8019b390(DB32(0x8020ef7c));
  FUN_8019b390(DB32(0x8020ef80));
  FUN_8019b390(DB32(0x8020ef84));
  FUN_8019b390(DB32(0x8020ef88));
  FUN_8019b390(DB32(0x8020ef8c));
  cd_pending_args_pump();
  FUN_8018e310(0);
}
