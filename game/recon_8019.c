/* recon_8019.c — clean-room reconstructions of guest functions in the
 * 0x8019xxxx band: the CD-driver response/wait/state-machine cluster.
 *
 * These are CPS coroutine ports in the cd_response_pump / battle_slot_draw
 * (recon_8016.c) style: the body resumes on cpu->pc via the entry switch,
 * every psx_check_interrupts_at is a hard IRQ delivery point, and external
 * jal/jalr targets tail-dispatch through cpu->pc so clean_try_dispatch
 * re-enters the callee and the callee's `jr $ra` lands back on the
 * continuation block. clean_cps.map routes the entry + every continuation
 * addr here; the cpst_<addr> transliterations in cd_irq_probe.c remain as
 * unreachable shadow code.
 *
 * All mutable state lives in cpu->gpr[]/guest RAM so scheduler longjmps
 * preserve it. Unlike the --timing cpst_ bodies these ports do NOT emit
 * per-instruction psx_cyc_* charges: guest time inside a port advances via
 * the tail-dispatched callees' own dispatch charges + the per-block IRQ
 * checks, the same contract the other clean CD ports (cd_busy_wait,
 * vsync_wait_n, cd_timer_read) already rely on.
 */
#include <stdint.h>
#include "cpu_state.h"

extern CPUState *clean_cpu_state(void);
extern void     psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);

/* MIPS GPR names — file-local aliases for cpu->gpr[N] (readability). */
#define zero cpu->gpr[0]
#define at   cpu->gpr[1]
#define v0   cpu->gpr[2]
#define v1   cpu->gpr[3]
#define a0   cpu->gpr[4]
#define a1   cpu->gpr[5]
#define a2   cpu->gpr[6]
#define a3   cpu->gpr[7]
#define t0   cpu->gpr[8]
#define t1   cpu->gpr[9]
#define t2   cpu->gpr[10]
#define t3   cpu->gpr[11]
#define t4   cpu->gpr[12]
#define t5   cpu->gpr[13]
#define t6   cpu->gpr[14]
#define t7   cpu->gpr[15]
#define s0   cpu->gpr[16]
#define s1   cpu->gpr[17]
#define s2   cpu->gpr[18]
#define s3   cpu->gpr[19]
#define s4   cpu->gpr[20]
#define s5   cpu->gpr[21]
#define s6   cpu->gpr[22]
#define s7   cpu->gpr[23]
#define t8   cpu->gpr[24]
#define t9   cpu->gpr[25]
#define k0   cpu->gpr[26]
#define k1   cpu->gpr[27]
#define gp   cpu->gpr[28]
#define sp   cpu->gpr[29]
#define fp   cpu->gpr[30]
#define ra   cpu->gpr[31]

/* ======== cd_result_wait_a_cps (guest 0x801968CC) ========
 * CD synchronous command-result wait (33 CPS blocks / 640 bytes).
 * a0 = wait mode (s6; 0 = retry until response, nonzero = single pass),
 * a1 = optional 8-byte result buffer guest addr (s7).
 *
 * Arms the shared busy-wait triple @0x801FC6E0/E4/E8 (deadline =
 * cd_timer_read(-1)+960, iteration counter, message ptr 0x8012582C),
 * then loops: cd_timer_read -> deadline/counter timeout check ->
 * cd_intc_busy_read -> while busy run the cd_response_process pump
 * (status bits 4->handlerA @0x801BC84C arg state@0x801BCB25 buf
 * 0x801FC6D0, bit 2->handlerB @0x801BC848 arg state@0x801BCB24 buf
 * 0x801FC6C8 — the same handler cells cd_response_pump dispatches) ->
 * when idle check the response byte @0x801BCB24: 2 or 5 = success
 * (commit 2, copy 8 bytes 0x801FC6C8 -> out_buf, return the byte),
 * else loop (mode 0) or return 0 (mode != 0). Timeout prints
 * "CD timeout"-class strings via cd_str_print + printf_forward_guest
 * then cd_cmd_state_reset and returns -1.
 *
 * Guest frame: sp-=64, saves s0-s7+fp+ra; all state in gpr[]/guest RAM.
 * 32 continuation addrs registered in clean_cps.map. */
uint32_t cd_result_wait_a_cps(void)
{
    CPUState *cpu = clean_cpu_state();
    if (!cpu) return 0;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80196908u: goto block_80196908;
            case 0x80196954u: goto block_80196954;
            case 0x801969A4u: goto block_801969A4;
            case 0x801969F0u: goto block_801969F0;
            case 0x801969F8u: goto block_801969F8;
            case 0x80196A14u: goto block_80196A14;
            case 0x80196A3Cu: goto block_80196A3C;
            case 0x80196A74u: goto block_80196A74;
            case 0x80196AA4u: goto block_80196AA4;
            case 0x8019694Cu: goto block_8019694C;
            case 0x8019696Cu: goto block_8019696C;
            case 0x80196998u: goto block_80196998;
            case 0x80196A00u: goto block_80196A00;
            case 0x80196A04u: goto block_80196A04;
            case 0x80196A0Cu: goto block_80196A0C;
            case 0x80196A1Cu: goto block_80196A1C;
            case 0x80196A34u: goto block_80196A34;
            case 0x80196A48u: goto block_80196A48;
            case 0x80196A50u: goto block_80196A50;
            case 0x80196A64u: goto block_80196A64;
            case 0x80196A78u: goto block_80196A78;
            case 0x80196A80u: goto block_80196A80;
            case 0x80196A94u: goto block_80196A94;
            case 0x80196AACu: goto block_80196AAC;
            case 0x80196ABCu: goto block_80196ABC;
            case 0x80196AD0u: goto block_80196AD0;
            case 0x80196AD8u: goto block_80196AD8;
            case 0x80196AF0u: goto block_80196AF0;
            case 0x80196AF4u: goto block_80196AF4;
            case 0x80196B0Cu: goto block_80196B0C;
            case 0x80196B14u: goto block_80196B14;
            case 0x80196B1Cu: goto block_80196B1C;
            default: break;
        }
    }
    /* Address: 0x801968CC, Size: 640 bytes, Blocks: 33 */

block_801968CC:
    /* entry: build 0x40 frame, save s0-s7/fp/ra, arm a0=-1 for the
     * cd_timer_read tail-call (jal link = block_80196908). */
    sp -= 64;
    cpu->write_word(sp + 48, s6);
    s6 = a0;
    cpu->write_word(sp + 52, s7);
    s7 = a1;
    a0 = 0xFFFFFFFFu;
    cpu->write_word(sp + 60, ra);
    cpu->write_word(sp + 56, fp);
    cpu->write_word(sp + 44, s5);
    cpu->write_word(sp + 40, s4);
    cpu->write_word(sp + 36, s3);
    cpu->write_word(sp + 32, s2);
    cpu->write_word(sp + 28, s1);
    ra = 0x80196908u;                       /* jal link */
    cpu->write_word(sp + 24, s0);           /* delay slot */
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u;                  /* tail-call cd_timer_read */
    return cpu->gpr[2];

block_80196908:
    /* cd_timer_read(-1) returned v0 = now. Arm the busy-wait triple and
     * precompute the cell/table base registers. */
    fp = 0x801BC86Cu;                       /* 0x801C0000-14228: name tbl2 base */
    s4 = 0x801BC8ECu;                       /* 0x801C0000-14100: name tbl1 base */
    s2 = 0x801BCB24u;                       /* 0x801C0000-13532: resp/state byte */
    s5 = s2 + 1;                            /* 0x801BCB25: state byte B */
    s3 = 2;                                 /* expected response / commit value */
    v0 += 960;                              /* deadline = now + 960 */
    cpu->write_word(0x801FC6E0, v0);        /* deadline cell */
    cpu->write_word(0x801FC6E4, zero);      /* iteration counter */
    cpu->write_word(0x801FC6E8, 0x8012582Cu); /* message ptr */
    /* fall through */

block_8019694C:
    /* outer wait-loop head: re-arm a0=-1 and tail-call cd_timer_read. */
    a0 = 0xFFFFFFFFu;
    ra = 0x80196954u;
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u;                  /* tail-call cd_timer_read */
    return cpu->gpr[2];

block_80196954:
    /* deadline < now ? (signed slt) -> timeout */
    v1 = cpu->read_word(0x801FC6E0);        /* deadline */
    v1 = (uint32_t)((int32_t)v1 < (int32_t)v0);
    if (v1 != zero) {
        psx_check_interrupts_at(cpu, 0x80196998u);
        goto block_80196998;                /* taken: timeout */
    }
    psx_check_interrupts_at(cpu, 0x8019696Cu);

block_8019696C:
    /* iteration counter++ ; cap 0x3C0000 exceeded -> timeout */
    v0 = cpu->read_word(0x801FC6E4);
    v1 = v0;
    v0 = v0 + 1;
    cpu->write_word(0x801FC6E4, v0);
    v0 = 0x003C0000u;
    v0 = (uint32_t)((int32_t)v0 < (int32_t)v1);
    if (v0 == zero) {                       /* cnt <= cap: continue */
        psx_check_interrupts_at(cpu, 0x80196A00u);
        goto block_80196A00;
    }
    psx_check_interrupts_at(cpu, 0x80196998u);
    goto block_80196998;                    /* timeout */

block_80196998:
    /* timeout: cd_str_print("CD timeout"-class msg 0x801257A4) */
    a0 = 0x801257A4u;
    ra = 0x801969A4u;
    psx_check_interrupts_at(cpu, 0x80197AD0u);
    cpu->pc = 0x80197AD0u;                  /* tail-call cd_str_print */
    return cpu->gpr[2];

block_801969A4:
    /* printf args: fmt 0x801257B4, a1=*0x801FC6E8, a2=name-tbl1[resp],
     * a3=name-tbl2[mode], stack arg = name-tbl1[byte2]. */
    a0 = cpu->read_byte(s2);                /* response byte */
    v0 = cpu->read_byte(s2 + 1);            /* state byte B */
    a1 = cpu->read_word(0x801FC6E8);        /* stored message ptr */
    v0 = v0 << 2;
    v0 = v0 + s4;
    a0 = a0 << 2;
    v1 = cpu->read_word(v0);                /* tbl1[byte2] -> stack arg */
    v0 = cpu->read_byte(0x801BC865u);       /* mode byte */
    a0 = a0 + s4;
    v0 = v0 << 2;
    v0 = v0 + fp;
    cpu->write_word(sp + 16, v1);           /* 5th printf arg on stack */
    a2 = cpu->read_word(a0);                /* tbl1[resp] */
    a3 = cpu->read_word(v0);                /* tbl2[mode] */
    a0 = 0x801257B4u;                       /* fmt */
    ra = 0x801969F0u;
    psx_check_interrupts_at(cpu, 0x8018E550u);
    cpu->pc = 0x8018E550u;                  /* tail-call printf_forward_guest */
    return cpu->gpr[2];

block_801969F0:
    /* cd_cmd_state_reset() then return -1 via block_80196A04. */
    ra = 0x801969F8u;
    psx_check_interrupts_at(cpu, 0x801972A8u);
    cpu->pc = 0x801972A8u;                  /* tail-call cd_cmd_state_reset */
    return cpu->gpr[2];

block_801969F8:
    /* j block_80196A04 with delay-slot v0 = -1 -> return -1. */
    v0 = 0xFFFFFFFFu;
    psx_check_interrupts_at(cpu, 0x80196A04u);
    goto block_80196A04;

block_80196A00:
    /* counter-under-cap path marker: v0 = 0 -> continue at 80196A04. */
    v0 = zero;
    /* fall through */

block_80196A04:
    /* v0 != 0 -> epilogue (return v0=-1); else -> intc busy check. */
    { int _bc = (v0 != zero);
      v0 = 0xFFFFFFFFu;                     /* delay slot */
      if (_bc) {
          psx_check_interrupts_at(cpu, 0x80196B1Cu);
          goto block_80196B1C;
      }
      psx_check_interrupts_at(cpu, 0x80196A0Cu);
    }

block_80196A0C:
    /* cd_intc_busy_read() -> v0 */
    ra = 0x80196A14u;
    psx_check_interrupts_at(cpu, 0x8019A7D4u);
    cpu->pc = 0x8019A7D4u;                  /* tail-call cd_intc_busy_read */
    return cpu->gpr[2];

block_80196A14:
    /* not busy -> check response byte; busy -> snapshot state&3 + pump. */
    if (v0 == zero) {
        psx_check_interrupts_at(cpu, 0x80196ABCu);
        goto block_80196ABC;
    }
    psx_check_interrupts_at(cpu, 0x80196A1Cu);

block_80196A1C:
    /* s1 = *(*0x801BCB0C) & 3 — saved low bits of the CD state byte. */
    v0 = cpu->read_word(0x801BCB0Cu);
    v0 = cpu->read_byte(v0);
    s1 = v0 & 3u;
    /* fall through */

block_80196A34:
    /* response-pump loop head: cd_response_process() -> status in v0. */
    ra = 0x80196A3Cu;
    psx_check_interrupts_at(cpu, 0x80196370u);
    cpu->pc = 0x80196370u;                  /* tail-call cd_response_process */
    return cpu->gpr[2];

block_80196A3C:
    /* status dispatch: 0 -> commit state byte; else bits 4/2 -> handlers. */
    s0 = v0;
    { int _bc = (s0 == zero);
      v0 = s0 & 4u;                         /* delay slot */
      if (_bc) {
          psx_check_interrupts_at(cpu, 0x80196AACu);
          goto block_80196AAC;
      }
      psx_check_interrupts_at(cpu, 0x80196A48u);
    }

block_80196A48:
    /* status & 4 ? -> handlerA arm check */
    { int _bc = (v0 == zero);               /* (s0&4)==0 */
      v0 = s0 & 2u;                         /* delay slot */
      if (_bc) {
          psx_check_interrupts_at(cpu, 0x80196A78u);
          goto block_80196A78;
      }
      psx_check_interrupts_at(cpu, 0x80196A50u);
    }

block_80196A50:
    /* handlerA cell @0x801BC84C (0x801C0000-14260) null ? -> bit-2 test */
    v0 = cpu->read_word(0x801BC84Cu);
    if (v0 == zero) {
        psx_check_interrupts_at(cpu, 0x80196A74u);
        goto block_80196A74;
    }
    psx_check_interrupts_at(cpu, 0x80196A64u);

block_80196A64:
    /* jalr handlerA(state@0x801BCB25, 0x801FC6D0) -> ret 0x80196A74 */
    a0 = cpu->read_byte(s5);
    a1 = 0x801FC6D0u;
    { uint32_t _jt = v0;                    /* handlerA fn-ptr */
      ra = 0x80196A74u;
      psx_check_interrupts_at(cpu, _jt);
      cpu->pc = _jt; return cpu->gpr[2]; }

block_80196A74:
    v0 = s0 & 2u;
    /* fall through */

block_80196A78:
    /* status & 2 clear -> pump again; set -> handlerB arm check */
    if (v0 == zero) {
        psx_check_interrupts_at(cpu, 0x80196A34u);
        goto block_80196A34;                /* loop: response_process */
    }
    psx_check_interrupts_at(cpu, 0x80196A80u);

block_80196A80:
    /* handlerB cell @0x801BC848 (0x801C0000-14264) null ? -> pump again */
    v0 = cpu->read_word(0x801BC848u);
    if (v0 == zero) {
        psx_check_interrupts_at(cpu, 0x80196A34u);
        goto block_80196A34;                /* loop */
    }
    psx_check_interrupts_at(cpu, 0x80196A94u);

block_80196A94:
    /* jalr handlerB(state@0x801BCB24, 0x801FC6C8) -> ret 0x80196AA4 */
    a0 = cpu->read_byte(s2);
    a1 = 0x801FC6C8u;
    { uint32_t _jt = v0;                    /* handlerB fn-ptr */
      ra = 0x80196AA4u;
      psx_check_interrupts_at(cpu, _jt);
      cpu->pc = _jt; return cpu->gpr[2]; }

block_80196AA4:
    /* j block_80196A34 — pump loop. */
    psx_check_interrupts_at(cpu, 0x80196A34u);
    goto block_80196A34;

block_80196AAC:
    /* response_process returned 0: commit saved low bits s1 back to
     * *(*0x801BCB0C), then check the response byte. */
    v0 = cpu->read_word(0x801BCB0Cu);
    cpu->write_byte(v0, (uint8_t)s1);
    /* fall through */

block_80196ABC:
    /* response byte @0x801BCB24 in {2,5} -> success; else give-up test. */
    v0 = cpu->read_byte(s2);
    a2 = v0 & 0x00FFu;
    v0 = 5;
    if (a2 == s3) {                         /* == 2 */
        psx_check_interrupts_at(cpu, 0x80196AD8u);
        goto block_80196AD8;
    }
    psx_check_interrupts_at(cpu, 0x80196AD0u);

block_80196AD0:
    /* == 5 ? -> success; else mode test */
    if (a2 != v0) {                         /* != 5 */
        psx_check_interrupts_at(cpu, 0x80196B14u);
        goto block_80196B14;
    }
    psx_check_interrupts_at(cpu, 0x80196AD8u);
    goto block_80196AD8;

block_80196AD8:
    /* success: mark resp=2, copy 8-byte result 0x801FC6C8 -> out_buf
     * when a1 != 0, return the response byte in v0 (=a2). */
    cpu->write_byte(s2, (uint8_t)s3);       /* *0x801BCB24 = 2 */
    a1 = s7;
    a0 = 0x801FC6C8u;
    v1 = 7;                                 /* delay slot: count = 7 */
    if (a1 == zero) {
        psx_check_interrupts_at(cpu, 0x80196B0Cu);
        goto block_80196B0C;
    }
    psx_check_interrupts_at(cpu, 0x80196AF0u);
    goto block_80196AF0;

block_80196AF0:
    a3 = 0xFFFFFFFFu;                       /* loop sentinel -1 */
    /* fall through */

block_80196AF4:
    /* 8-byte copy loop: *a1++ = *a0++ while --v1 != -1. */
    v0 = cpu->read_byte(a0);
    a0 = a0 + 1;
    v1 = v1 - 1;
    cpu->write_byte(a1, (uint8_t)v0);
    a1 = a1 + 1;                            /* delay slot */
    if (v1 != a3) {
        psx_check_interrupts_at(cpu, 0x80196AF4u);
        goto block_80196AF4;
    }
    psx_check_interrupts_at(cpu, 0x80196B0Cu);
    goto block_80196B0C;

block_80196B0C:
    v0 = a2;                                /* return response byte */
    psx_check_interrupts_at(cpu, 0x80196B1Cu);
    goto block_80196B1C;

block_80196B14:
    /* response byte unexpected: mode==0 -> outer wait loop; else ret 0. */
    v0 = zero;                              /* delay slot */
    if (s6 == zero) {
        psx_check_interrupts_at(cpu, 0x8019694Cu);
        goto block_8019694C;
    }
    psx_check_interrupts_at(cpu, 0x80196B1Cu);
    goto block_80196B1C;

block_80196B1C:
    /* epilogue: restore s0-s7/fp/ra, sp+=64, jr ra. */
    ra = cpu->read_word(sp + 60);
    fp = cpu->read_word(sp + 56);
    s7 = cpu->read_word(sp + 52);
    s6 = cpu->read_word(sp + 48);
    s5 = cpu->read_word(sp + 44);
    s4 = cpu->read_word(sp + 40);
    s3 = cpu->read_word(sp + 36);
    s2 = cpu->read_word(sp + 32);
    s1 = cpu->read_word(sp + 28);
    s0 = cpu->read_word(sp + 24);
    sp += 64;                               /* delay slot */
    { uint32_t _jt = ra;
      psx_check_interrupts_at(cpu, _jt);
      cpu->pc = _jt; return cpu->gpr[2]; }
}

/* ======== cd_result_wait_b_cps (guest 0x80196B4C) ========
 * CD synchronous two-slot result wait (37 CPS blocks / 712 bytes).
 * a0 = wait mode (s7; 0 = retry until a result lands, nonzero = single
 * pass), a1 = optional out_buf guest addr (s3).
 *
 * Same busy-wait triple @0x801FC6E0/E4/E8 as wait_a (deadline +960,
 * counter cap 0x3C0000, message ptr 0x80125834) and the same
 * cd_response_process pump (status bit4 -> handlerA @0x801BC84C arg
 * state@0x801BCB25 buf 0x801FC6D0, bit2 -> handlerB @0x801BC848 arg
 * state@0x801BCB24 buf 0x801FC6C8). On idle it polls TWO response
 * slots: *0x801BCB26 != 0 -> consume (write 0), copy 8 bytes
 * 0x801FC6D8 -> out_buf, return the slot byte; elif *0x801BCB25 != 0 ->
 * consume, copy 8 bytes 0x801FC6D0 -> out_buf, return the byte; else
 * mode==0 loops / mode!=0 returns 0. Timeout path identical shape to
 * wait_a (cd_str_print + printf_forward_guest + cd_cmd_state_reset,
 * return -1).
 * Guest frame sp-=64 saves s0-s7+fp+ra; 36 continuation addrs. */
uint32_t cd_result_wait_b_cps(void)
{
    CPUState *cpu = clean_cpu_state();
    if (!cpu) return 0;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80196B88u: goto block_80196B88;
            case 0x80196BD4u: goto block_80196BD4;
            case 0x80196C24u: goto block_80196C24;
            case 0x80196C70u: goto block_80196C70;
            case 0x80196C78u: goto block_80196C78;
            case 0x80196C94u: goto block_80196C94;
            case 0x80196CBCu: goto block_80196CBC;
            case 0x80196CF4u: goto block_80196CF4;
            case 0x80196D24u: goto block_80196D24;
            case 0x80196BCCu: goto block_80196BCC;
            case 0x80196BECu: goto block_80196BEC;
            case 0x80196C18u: goto block_80196C18;
            case 0x80196C80u: goto block_80196C80;
            case 0x80196C84u: goto block_80196C84;
            case 0x80196C8Cu: goto block_80196C8C;
            case 0x80196C9Cu: goto block_80196C9C;
            case 0x80196CB4u: goto block_80196CB4;
            case 0x80196CC8u: goto block_80196CC8;
            case 0x80196CD0u: goto block_80196CD0;
            case 0x80196CE4u: goto block_80196CE4;
            case 0x80196CF8u: goto block_80196CF8;
            case 0x80196D00u: goto block_80196D00;
            case 0x80196D14u: goto block_80196D14;
            case 0x80196D2Cu: goto block_80196D2C;
            case 0x80196D3Cu: goto block_80196D3C;
            case 0x80196D50u: goto block_80196D50;
            case 0x80196D64u: goto block_80196D64;
            case 0x80196D6Cu: goto block_80196D6C;
            case 0x80196D84u: goto block_80196D84;
            case 0x80196D8Cu: goto block_80196D8C;
            case 0x80196DA0u: goto block_80196DA0;
            case 0x80196DB8u: goto block_80196DB8;
            case 0x80196DBCu: goto block_80196DBC;
            case 0x80196DD4u: goto block_80196DD4;
            case 0x80196DDCu: goto block_80196DDC;
            case 0x80196DE4u: goto block_80196DE4;
            default: break;
        }
    }
    /* Address: 0x80196B4C, Size: 712 bytes, Blocks: 37 */

block_80196B4C:
    /* entry: frame + save s0-s7/fp/ra; s7=a0 mode, s3=a1 out_buf. */
    sp -= 64;
    cpu->write_word(sp + 52, s7);
    s7 = a0;
    cpu->write_word(sp + 36, s3);
    s3 = a1;
    a0 = 0xFFFFFFFFu;
    cpu->write_word(sp + 60, ra);
    cpu->write_word(sp + 56, fp);
    cpu->write_word(sp + 48, s6);
    cpu->write_word(sp + 44, s5);
    cpu->write_word(sp + 40, s4);
    cpu->write_word(sp + 32, s2);
    cpu->write_word(sp + 28, s1);
    ra = 0x80196B88u;                       /* jal link */
    cpu->write_word(sp + 24, s0);           /* delay slot */
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u;                  /* tail-call cd_timer_read */
    return cpu->gpr[2];

block_80196B88:
    /* arm wait triple; s2=resp base 0x801BCB24, s6=+1, s4=+2,
     * s5=name tbl1 0x801BC8EC, fp=name tbl2 0x801BC86C. */
    fp = 0x801BC86Cu;
    s5 = 0x801BC8ECu;
    s2 = 0x801BCB24u;
    s6 = s2 + 1;                            /* 0x801BCB25 slot1 */
    s4 = s2 + 2;                            /* 0x801BCB26 slot2 */
    v0 += 960;
    cpu->write_word(0x801FC6E0, v0);        /* deadline */
    cpu->write_word(0x801FC6E4, zero);      /* counter */
    cpu->write_word(0x801FC6E8, 0x80125834u); /* message ptr */
    /* fall through */

block_80196BCC:
    /* outer wait-loop head: cd_timer_read(-1). */
    a0 = 0xFFFFFFFFu;
    ra = 0x80196BD4u;
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u;
    return cpu->gpr[2];

block_80196BD4:
    /* deadline < now -> timeout */
    v1 = cpu->read_word(0x801FC6E0);
    v1 = (uint32_t)((int32_t)v1 < (int32_t)v0);
    if (v1 != zero) {
        psx_check_interrupts_at(cpu, 0x80196C18u);
        goto block_80196C18;
    }
    psx_check_interrupts_at(cpu, 0x80196BECu);

block_80196BEC:
    /* counter++ ; cap -> timeout */
    v0 = cpu->read_word(0x801FC6E4);
    v1 = v0;
    v0 = v0 + 1;
    cpu->write_word(0x801FC6E4, v0);
    v0 = 0x003C0000u;
    v0 = (uint32_t)((int32_t)v0 < (int32_t)v1);
    if (v0 == zero) {
        psx_check_interrupts_at(cpu, 0x80196C80u);
        goto block_80196C80;
    }
    psx_check_interrupts_at(cpu, 0x80196C18u);
    goto block_80196C18;

block_80196C18:
    /* timeout: cd_str_print(0x801257A4) */
    a0 = 0x801257A4u;
    ra = 0x80196C24u;
    psx_check_interrupts_at(cpu, 0x80197AD0u);
    cpu->pc = 0x80197AD0u;
    return cpu->gpr[2];

block_80196C24:
    /* printf args (same shape as wait_a; tbl1 base = s5). */
    a0 = cpu->read_byte(s2);
    v0 = cpu->read_byte(s2 + 1);
    a1 = cpu->read_word(0x801FC6E8);
    v0 = v0 << 2;
    v0 = v0 + s5;
    a0 = a0 << 2;
    v1 = cpu->read_word(v0);
    v0 = cpu->read_byte(0x801BC865u);
    a0 = a0 + s5;
    v0 = v0 << 2;
    v0 = v0 + fp;
    cpu->write_word(sp + 16, v1);
    a2 = cpu->read_word(a0);
    a3 = cpu->read_word(v0);
    a0 = 0x801257B4u;
    ra = 0x80196C70u;
    psx_check_interrupts_at(cpu, 0x8018E550u);
    cpu->pc = 0x8018E550u;
    return cpu->gpr[2];

block_80196C70:
    /* cd_cmd_state_reset() then return -1 via 0x80196C84. */
    ra = 0x80196C78u;
    psx_check_interrupts_at(cpu, 0x801972A8u);
    cpu->pc = 0x801972A8u;
    return cpu->gpr[2];

block_80196C78:
    v0 = 0xFFFFFFFFu;                       /* j 0x80196C84, v0=-1 */
    psx_check_interrupts_at(cpu, 0x80196C84u);
    goto block_80196C84;

block_80196C80:
    v0 = zero;                              /* continue marker */
    /* fall through */

block_80196C84:
    /* v0 != 0 -> epilogue (ret -1); else intc busy check. */
    { int _bc = (v0 != zero);
      v0 = 0xFFFFFFFFu;
      if (_bc) {
          psx_check_interrupts_at(cpu, 0x80196DE4u);
          goto block_80196DE4;
      }
      psx_check_interrupts_at(cpu, 0x80196C8Cu);
    }

block_80196C8C:
    /* cd_intc_busy_read() */
    ra = 0x80196C94u;
    psx_check_interrupts_at(cpu, 0x8019A7D4u);
    cpu->pc = 0x8019A7D4u;
    return cpu->gpr[2];

block_80196C94:
    /* not busy -> poll result slots; busy -> snapshot + pump. */
    if (v0 == zero) {
        psx_check_interrupts_at(cpu, 0x80196D3Cu);
        goto block_80196D3C;
    }
    psx_check_interrupts_at(cpu, 0x80196C9Cu);

block_80196C9C:
    /* s1 = *(*0x801BCB0C) & 3 */
    v0 = cpu->read_word(0x801BCB0Cu);
    v0 = cpu->read_byte(v0);
    s1 = v0 & 3u;
    /* fall through */

block_80196CB4:
    /* pump head: cd_response_process() */
    ra = 0x80196CBCu;
    psx_check_interrupts_at(cpu, 0x80196370u);
    cpu->pc = 0x80196370u;
    return cpu->gpr[2];

block_80196CBC:
    /* status dispatch (same bit layout as wait_a). */
    s0 = v0;
    { int _bc = (s0 == zero);
      v0 = s0 & 4u;
      if (_bc) {
          psx_check_interrupts_at(cpu, 0x80196D2Cu);
          goto block_80196D2C;
      }
      psx_check_interrupts_at(cpu, 0x80196CC8u);
    }

block_80196CC8:
    { int _bc = (v0 == zero);               /* (s0&4)==0 */
      v0 = s0 & 2u;
      if (_bc) {
          psx_check_interrupts_at(cpu, 0x80196CF8u);
          goto block_80196CF8;
      }
      psx_check_interrupts_at(cpu, 0x80196CD0u);
    }

block_80196CD0:
    /* handlerA cell @0x801BC84C (0x801C0000-14260) */
    v0 = cpu->read_word(0x801BC84Cu);
    if (v0 == zero) {
        psx_check_interrupts_at(cpu, 0x80196CF4u);
        goto block_80196CF4;
    }
    psx_check_interrupts_at(cpu, 0x80196CE4u);

block_80196CE4:
    /* jalr handlerA(*0x801BCB25, 0x801FC6D0) -> ret 0x80196CF4 */
    a0 = cpu->read_byte(s6);
    a1 = 0x801FC6D0u;
    { uint32_t _jt = v0;
      ra = 0x80196CF4u;
      psx_check_interrupts_at(cpu, _jt);
      cpu->pc = _jt; return cpu->gpr[2]; }

block_80196CF4:
    v0 = s0 & 2u;
    /* fall through */

block_80196CF8:
    if (v0 == zero) {                       /* (s0&2)==0 -> pump again */
        psx_check_interrupts_at(cpu, 0x80196CB4u);
        goto block_80196CB4;
    }
    psx_check_interrupts_at(cpu, 0x80196D00u);

block_80196D00:
    /* handlerB cell @0x801BC848 (0x801C0000-14264) */
    v0 = cpu->read_word(0x801BC848u);
    if (v0 == zero) {
        psx_check_interrupts_at(cpu, 0x80196CB4u);
        goto block_80196CB4;
    }
    psx_check_interrupts_at(cpu, 0x80196D14u);

block_80196D14:
    /* jalr handlerB(*0x801BCB24, 0x801FC6C8) -> ret 0x80196D24 */
    a0 = cpu->read_byte(s2);
    a1 = 0x801FC6C8u;
    { uint32_t _jt = v0;
      ra = 0x80196D24u;
      psx_check_interrupts_at(cpu, _jt);
      cpu->pc = _jt; return cpu->gpr[2]; }

block_80196D24:
    psx_check_interrupts_at(cpu, 0x80196CB4u);
    goto block_80196CB4;                    /* pump loop */

block_80196D2C:
    /* status==0: commit s1 -> *(*0x801BCB0C), then poll slots. */
    v0 = cpu->read_word(0x801BCB0Cu);
    cpu->write_byte(v0, (uint8_t)s1);
    /* fall through */

block_80196D3C:
    /* slot2 *0x801BCB26 nonzero? -> consume + copy 0x801FC6D8 */
    v0 = cpu->read_byte(s4);
    a2 = v0 & 0x00FFu;
    if (a2 == zero) {
        psx_check_interrupts_at(cpu, 0x80196D8Cu);
        goto block_80196D8C;
    }
    psx_check_interrupts_at(cpu, 0x80196D50u);

block_80196D50:
    /* consume slot2, copy 8B 0x801FC6D8 -> s3 (if s3 != 0). */
    cpu->write_byte(s2 + 2, (uint8_t)zero); /* *0x801BCB26 = 0 */
    a0 = 0x801FC6D8u;
    a1 = s3;
    if (s3 == zero) {
        psx_check_interrupts_at(cpu, 0x80196DD4u);
        goto block_80196DD4;
    }
    psx_check_interrupts_at(cpu, 0x80196D64u);
    goto block_80196D64;

block_80196D64:
    v1 = 7;                                 /* 0x80196D64: count = 7 */
    a3 = 0xFFFFFFFFu;                       /* 0x80196D68: sentinel -1 */
    /* fall through */

block_80196D6C:
    /* 8-byte copy loop: *a1++ = *a0++ while --v1 != -1. */
    v0 = cpu->read_byte(a0);
    a0 = a0 + 1;
    v1 = v1 - 1;
    cpu->write_byte(a1, (uint8_t)v0);
    a1 = a1 + 1;                            /* delay slot */
    if (v1 != a3) {
        psx_check_interrupts_at(cpu, 0x80196D6Cu);
        goto block_80196D6C;
    }
    psx_check_interrupts_at(cpu, 0x80196D84u);
    goto block_80196D84;

block_80196D84:
    v0 = a2;                                /* return slot byte */
    psx_check_interrupts_at(cpu, 0x80196DE4u);
    goto block_80196DE4;

block_80196D8C:
    /* slot1 *0x801BCB25 nonzero? -> consume + copy 0x801FC6D0 */
    v0 = cpu->read_byte(s4 - 1);
    a2 = v0 & 0x00FFu;
    if (a2 == zero) {
        psx_check_interrupts_at(cpu, 0x80196DDCu);
        goto block_80196DDC;
    }
    psx_check_interrupts_at(cpu, 0x80196DA0u);

block_80196DA0:
    /* consume slot1, copy 8B 0x801FC6D0 -> s3 (if s3 != 0). */
    cpu->write_byte(s2 + 1, (uint8_t)zero); /* *0x801BCB25 = 0 */
    a1 = s3;
    a0 = 0x801FC6D0u;
    v1 = 7;                                 /* delay slot */
    if (a1 == zero) {
        psx_check_interrupts_at(cpu, 0x80196DD4u);
        goto block_80196DD4;
    }
    psx_check_interrupts_at(cpu, 0x80196DB8u);
    goto block_80196DB8;

block_80196DB8:
    a3 = 0xFFFFFFFFu;
    /* fall through */

block_80196DBC:
    /* 8-byte copy loop: *a1++ = *a0++ while --v1 != -1. */
    v0 = cpu->read_byte(a0);
    a0 = a0 + 1;
    v1 = v1 - 1;
    cpu->write_byte(a1, (uint8_t)v0);
    a1 = a1 + 1;                            /* delay slot */
    if (v1 != a3) {
        psx_check_interrupts_at(cpu, 0x80196DBCu);
        goto block_80196DBC;
    }
    psx_check_interrupts_at(cpu, 0x80196DD4u);
    goto block_80196DD4;

block_80196DD4:
    v0 = a2;                                /* return slot byte */
    psx_check_interrupts_at(cpu, 0x80196DE4u);
    goto block_80196DE4;

block_80196DDC:
    /* no slot pending: mode==0 -> outer wait loop; else ret 0. */
    v0 = zero;                              /* delay slot */
    if (s7 == zero) {
        psx_check_interrupts_at(cpu, 0x80196BCCu);
        goto block_80196BCC;
    }
    psx_check_interrupts_at(cpu, 0x80196DE4u);
    goto block_80196DE4;

block_80196DE4:
    /* epilogue: restore s0-s7/fp/ra, sp+=64, jr ra. */
    ra = cpu->read_word(sp + 60);
    fp = cpu->read_word(sp + 56);
    s7 = cpu->read_word(sp + 52);
    s6 = cpu->read_word(sp + 48);
    s5 = cpu->read_word(sp + 44);
    s4 = cpu->read_word(sp + 40);
    s3 = cpu->read_word(sp + 36);
    s2 = cpu->read_word(sp + 32);
    s1 = cpu->read_word(sp + 28);
    s0 = cpu->read_word(sp + 24);
    sp += 64;                               /* delay slot */
    { uint32_t _jt = ra;
      psx_check_interrupts_at(cpu, _jt);
      cpu->pc = _jt; return cpu->gpr[2]; }
}
