/* resolve.c — FUN_80176248 action-request resolver.
 * Faithful port: guest decompile kept 1:1 (identifiers, goto, labels).
 * Types: byte/ushort/uint/undefinedN are guest-width typedefs.
 * DAT_ symbols are lvalue macros yielding guest-field pointers;
 * _DAT_801f04a4 is an int32_t* guest pointer. FUN_ calls bind to
 * host hooks (declared below; guest names preserved for audit). */
#include <stdint.h>
#include <stdbool.h>

typedef uint8_t  byte;
typedef uint16_t ushort;
typedef uint32_t uint;
typedef uint16_t undefined2;
typedef uint32_t undefined4;
typedef uint8_t  undefined;
typedef uint8_t  undefined1;
typedef uint64_t undefined8;
typedef uint64_t ulonglong;
#define CONCAT44(hi,lo) (((uint64_t)(uint32_t)(hi) << 32) | (uint32_t)(lo))

/* guest absolute-address globals (host binds storage) */
extern uint8_t *g_DAT_801f4d70, *g_DAT_801f4d72, *g_DAT_801f4d76;
extern uint8_t *g_DAT_801f596c, *g_DAT_801f596d, *g_DAT_801f5970, *g_DAT_801f5971;
extern uint8_t *g_DAT_801b55d0, *g_DAT_801f04c9;
extern uint16_t *g_DAT_801f04ec, *g_DAT_801f04ee, *g_DAT_801f04f0,
              *g_DAT_801f04f2, *g_DAT_801f04f4, *g_DAT_801f04f6,
              *g_DAT_801f04f8, *g_DAT_801f04fa;
extern int32_t *g__DAT_801f04a4;
/* _DAT_801f04a4 is the guest POINTER stored in the 4-byte cell at
 * 0x801f04a4 (written by record_table_select as a guest address).
 * g__DAT_801f04a4 is the host pointer to that cell; dereference the
 * u32 guest-pointer cell and translate before indexing the pointee. */
#define _DAT_801f04a4 ((int32_t *)guest_at(*(uint32_t *)g__DAT_801f04a4))
#define _DAT_801f4d70   (*g_DAT_801f4d70)
#define _DAT_801f4d72   (*g_DAT_801f4d72)
#define _DAT_801f4d76   (*g_DAT_801f4d76)
#define _DAT_801f596c   (*g_DAT_801f596c)
#define _DAT_801f596d   (*g_DAT_801f596d)
#define _DAT_801f5970   (*g_DAT_801f5970)
#define _DAT_801f5971   (*g_DAT_801f5971)
#define _DAT_801b55d0   (*g_DAT_801b55d0)
#define _DAT_801f04c9   (*g_DAT_801f04c9)
#define _DAT_801f04ec   (*g_DAT_801f04ec)
#define _DAT_801f04ee   (*g_DAT_801f04ee)
#define _DAT_801f04f0   (*g_DAT_801f04f0)
#define _DAT_801f04f2   (*g_DAT_801f04f2)
#define _DAT_801f04f4   (*g_DAT_801f04f4)
#define _DAT_801f04f6   (*g_DAT_801f04f6)
#define _DAT_801f04f8   (*g_DAT_801f04f8)
#define _DAT_801f04fa   (*g_DAT_801f04fa)
#define DAT_801f4d70   (*g_DAT_801f4d70)
#define DAT_801f4d72   (*g_DAT_801f4d72)
#define DAT_801f4d76   (*g_DAT_801f4d76)
#define DAT_801f596c   (*g_DAT_801f596c)
#define DAT_801f596d   (*g_DAT_801f596d)
#define DAT_801f5970   (*g_DAT_801f5970)
#define DAT_801f5971   (*g_DAT_801f5971)
#define DAT_801b55d0   (*g_DAT_801b55d0)
#define DAT_801f04c9   (*g_DAT_801f04c9)
#define DAT_801f04ec   (*g_DAT_801f04ec)
#define DAT_801f04ee   (*g_DAT_801f04ee)
#define DAT_801f04f0   (*g_DAT_801f04f0)
#define DAT_801f04f2   (*g_DAT_801f04f2)
#define DAT_801f04f4   (*g_DAT_801f04f4)
#define DAT_801f04f6   (*g_DAT_801f04f6)
#define DAT_801f04f8   (*g_DAT_801f04f8)
#define DAT_801f04fa   (*g_DAT_801f04fa)

/* additional guest globals (batch3 ports) */
extern uint8_t *g_DAT_80123844, *g_DAT_801247a8;
extern uint8_t *g_DAT_801f3600, *g_DAT_801f36ba;
extern uint8_t *g_DAT_801f0db0, *g_DAT_801f0db4, *g_DAT_801f0ae0, *g_DAT_801f0ae4;
extern uint8_t *g_DAT_801f0ad8, *g_DAT_1f8000b1, *g_DAT_801b1d48;
extern uint16_t *g_DAT_1f80038c;
extern int32_t *g_DAT_801f35f4i;
#define DAT_80123844   (*g_DAT_80123844)
#define DAT_801247a8   (*g_DAT_801247a8)
#define _DAT_801f35f4  (*g_DAT_801f35f4i)
#define DAT_801f3600   (*g_DAT_801f3600)
#define DAT_801f0db0   (*g_DAT_801f0db0)
#define DAT_801f0db4   (*g_DAT_801f0db4)
#define DAT_801f0ae0   (*g_DAT_801f0ae0)
#define DAT_801f0ae4   (*g_DAT_801f0ae4)
#define DAT_801f0ad8   (*g_DAT_801f0ad8)
#define DAT_1f8000b1   (*g_DAT_1f8000b1)
#define DAT_801b1d48   (*g_DAT_801b1d48)
#define DAT_801f36ba   (*g_DAT_801f36ba)
#define _DAT_1f80038c  g_DAT_1f80038c[0]
#define _DAT_1f80038e  g_DAT_1f80038c[1]
#define _DAT_1f800390  g_DAT_1f80038c[2]
#define DAT_1f80038c   g_DAT_1f80038c[0]
#define DAT_1f80038e   g_DAT_1f80038c[1]
#define DAT_1f800390   g_DAT_1f80038c[2]

/* guest-address <-> host-pointer bridge (bind.c) */
extern void *guest_at(uint32_t addr);
extern uint32_t guest_of(const void *p);

/* host hooks for guest calls (bound by runtime) */
extern void pending_hit_a_clear(byte*);
void trig_lut_read(uint32_t,byte*);
void trig_vel_flat(uint32_t,byte*);
void trig_vel_up(uint32_t,byte*);
void trig_vel_diag(uint32_t,byte*);
extern void hit_chain_clear(byte*);
extern void pending_hit_b_clear(byte*);
extern void fighter_combat_reset(byte*);
extern int32_t pending_action_gate(uint32_t, byte*, uint32_t, uint32_t);
void slot_mark2_set(int);
int lut_cos_q12(int);
int lut_sin_q12(uint);
void deferred_cmd_reset(void);
undefined8 vec2_rot_lut(undefined4,int);
void dir_rotate_acc(int,int,byte*,byte*);
void dir_rotate_set(int,int,byte*,byte*);

/* Faithful signed 32/32 `div` LO result. Guest div-by-zero yields
 * LO = (dividend<0) ? 1 : -1 (no trap); INT_MIN/-1 yields INT_MIN.
 * Use wherever the guest emits `div`/`break 7`-free signed divide. */
static int32_t mips_div_lo(int32_t num, int32_t den)
{
    if (den == 0) return num >= 0 ? -1 : 1;
    if (num == (int32_t)0x80000000 && den == -1) return (int32_t)0x80000000;
    return num / den;
}

/* action_resolve - pick next fighter action from input/state flag
 * words; returns action id (or -1 = keep current). */
int32_t action_resolve(byte *ctrl, uint8_t *motion)
{
  /* Locals: fXX = snapshot of the ctrl+0xXX flag word (0x58 request,
   * 0x5c state, 0x60/0x64 mode, 0x68 temp, 0x6c motion-state).
   * btn/ba/bb = button masks; cat/cat2 = direction nibbles from
   * ctrl+0x7c/+0x82; act/act2 = primary/secondary action candidates;
   * motion_p = ctrl+0x38 motion object; t2 = ctrl+0x2c foe ctrl;
   * cnt = ctrl+0xce timer; sv/w/zb/idx = scratch. */
  short sv;
  ushort btn;
  ushort cnt;
  bool cancel;
  ushort bb;
  byte idx;
  undefined2 w;
  int zb;
  ushort cat2;
  uint f6c;
  uint f60;
  int t2;
  uint f64;
  uint f68;
  int act;
  uint f58;
  int act2;
  uintptr_t motion_p;
  ushort cat;
  ushort ba;
  uint f5c;
  
  act2 = -1;
  act = -1;
  btn = *(ushort *)(ctrl + 0x7c);
  f68 = *(uint *)(ctrl + 0x68);
  motion_p = (uintptr_t)guest_at(*(uint32_t *)(ctrl + 0x38));
  t2 = *(uint32_t *)(ctrl + 0x2c);
  cat = btn & 0xf000;
  f58 = *(uint *)(ctrl + 0x58);
  f5c = *(uint *)(ctrl + 0x5c);
  f60 = *(uint *)(ctrl + 0x60);
  f64 = *(uint *)(ctrl + 100);
  f6c = *(uint *)(ctrl + 0x6c);
  cat2 = *(ushort *)(ctrl + 0x82) & 0xf000;
  if ((ctrl[5] & 1) == 0) {
    ba = *(ushort *)(ctrl + 0x80) & 0x4c8;
    bb = *(ushort *)(ctrl + 0x84) & 0x4c8;
  }
  else {
    ba = *(ushort *)(ctrl + 0x80) & 0x4e8;
    bb = *(ushort *)(ctrl + 0x84) & 0x4e8;
  }
  cnt = *(ushort *)(ctrl + 0xce);
  if ((0 < *(short *)(ctrl + 0xce)) &&
     (*(short *)(ctrl + 0xce) = (short)((uint)cnt - (uint)*(ushort *)(ctrl + 0x88)),
     (int)(((uint)cnt - (uint)*(ushort *)(ctrl + 0x88)) * 0x10000) < 0)) {
    ctrl[0xce] = 0;
    ctrl[0xcf] = 0;
  }
  /* juggle/launch pin: zero x-vel, force hop velocity, pick juggle
   * action (0x6b-0x70) by turn rate sign/magnitude. */
  if ((f6c & 0x100000) != 0) {
    if (*(short *)(motion_p + 0xd6) < 0x100) {
      *(undefined2 *)(motion_p + 0xd6) = 0;
    }
    ctrl[0x90] = 0;
    ctrl[0x91] = 0;
    if (*(short *)(ctrl + 0x92) == 0) {
      ctrl[0x92] = 0xa0;
      ctrl[0x93] = 0;
    }
    ctrl[0xb2] = 0;
    ctrl[0xb3] = 0xec;
    ctrl[0xb4] = 0x78;
    ctrl[0xb5] = 0;
    f64 = *(uint *)(ctrl + 0x6c);
    f60 = f64 & 0xfff7ffff;
    *(short *)(ctrl + 0xc4) = *(short *)(ctrl + 0x4e) + *(short *)(ctrl + 0x70);
    *(uint *)(ctrl + 0x6c) = f60;
    if ((f6c & 0x80000) == 0) {
      *(uint *)(ctrl + 0x6c) = f64 & 0xffe7ffff;
      if ((f58 & 0x20000) != 0) {
        act2 = (int)*(short *)(ctrl + 0x70);
        if (act2 < 0) {
          if (-act2 < 0x400) {
            return 0x6d;
          }
          return 0x6e;
        }
        if (act2 < 0x400) {
          return 0x6d;
        }
        return 0x6e;
      }
      if ((f58 & 0x10000) == 0) {
        act2 = (int)*(short *)(ctrl + 0x70);
        if (act2 < 0) {
          if (-act2 < 0x400) {
            return 0x6b;
          }
          return 0x6c;
        }
        if (act2 < 0x400) {
          return 0x6b;
        }
        return 0x6c;
      }
      act2 = (int)*(short *)(ctrl + 0x70);
      if (act2 < 0) {
        if (-act2 < 0x400) {
          return 0x70;
        }
        return 0x6f;
      }
      if (act2 < 0x400) {
        return 0x70;
      }
      return 0x6f;
    }
    goto LAB_80176d24;
  }
  /* pending-hit window: return stored follow-up action from
   * motion_p+0x24 after clearing hit records. */
  if ((f6c & 0x400000) != 0) {
    *(uint *)(ctrl + 0x6c) = *(uint *)(ctrl + 0x6c) & 0xffbfffff;
    sv = *(short *)(motion_p + 0x24);
    *(undefined2 *)(motion_p + 0x24) = 0xffff;
    *(undefined2 *)(motion_p + 0x2a) = 0xffff;
    pending_hit_a_clear(ctrl);
    hit_chain_clear(ctrl);
    return sv + 0x90;
  }
  /* launched/hitstun: decay timer, reset or clear hits, return
   * action from the motion_p+4 table indexed by +0x2c. */
  if ((f6c & 0x800000) != 0) {
    ctrl[0xd4] = 0;
    ctrl[0xd5] = 0;
    *(uint *)(ctrl + 0x6c) = *(uint *)(ctrl + 0x6c) & 0xff7fffff;
    if ((f6c & 0x8000) == 0) {
      pending_hit_a_clear(ctrl);
      hit_chain_clear(ctrl);
    }
    else {
      fighter_combat_reset(ctrl);
    }
    *(undefined2 *)(&DAT_801f4d70 + (uint)ctrl[2] * 0xbc) = 0;
    *(undefined2 *)(&DAT_801f4d72 + (uint)ctrl[2] * 0xbc) = 0;
    *(undefined2 *)(&DAT_801f4d76 + (uint)ctrl[2] * 0xbc) = 0xfff6;
    return *(short *)guest_at((uint32_t)(*(short *)(motion_p + 0x2c) * 2 + *(int *)(motion_p + 4) + -0x1a0)) + 0x90;
  }
  /* main dispatch: f58 request-flag word (sign clear = normal
   * path; the nested tree maps flag combos to action ids). */
  if ((f58 & 0x40000000) == 0) {
    if ((f58 & 0x100000) == 0) {
      if ((f5c & 0x4000000) != 0) goto LAB_801766e8;
      if ((f6c & 0x80000) != 0) {
        if ((f58 & 0x3000) == 0x3000) {
          if (((((*(ushort *)(ctrl + 0x7c) & 0x800) != 0) && ((f60 & 0x1000000) == 0)) &&
              ((f5c & 0x1400) == 0x1000)) && ((btn & 0x2000) == 0)) {
            if ((*(ushort *)(ctrl + 0x7c) & 0x400) != 0) {
              *(uint *)(ctrl + 0x5c) = *(uint *)(ctrl + 0x5c) | 2;
            }
            *(uint *)(ctrl + 0x5c) = *(uint *)(ctrl + 0x5c) | 1;
          }
          *(uint *)(ctrl + 0x5c) = *(uint *)(ctrl + 0x5c) | 0x4000;
        }
        goto LAB_80176d18;
      }
      if (((f58 & 0x10000000) == 0) && ((f5c & 0x2000000) == 0)) {
        if (((f58 & 0x40000) == 0) && ((f58 & 0x40) == 0)) {
          if ((f58 & 0x6000000) == 0) {
            if (((f58 & 0x30000) == 0) || ((f60 & 0x20) != 0)) {
              if (((f58 & 0x8000) == 0) &&
                 ((((f58 & 0x80000) == 0 && ((f58 & 0x1000000) == 0)) && (-1 < (int)f58))))
              {
                if ((f58 & 0x1000) == 0) {
                  if ((f58 & 0x200) == 0) {
                    if ((f58 & 0x400) == 0) {
                      if ((f58 & 0x20) == 0) {
                        if ((f58 & 0x200000) != 0) {
                          return -1;
                        }
                        if ((f60 & 1) == 0) {
                          act = 1;
                          if (0 < *(short *)(ctrl + 0xce)) {
                            return 1;
                          }
                          f68 = *(uint *)(ctrl + 0x5c);
                          ctrl[0xc0] = 0;
                          ctrl[0xc1] = 0;
                          *(uint *)(ctrl + 0x5c) = f68 | 0x10000000;
                          if (((((*(ushort *)(ctrl + 0x7c) & 0x800) != 0) &&
                               ((f60 & 0x1000000) == 0)) && ((f5c & 0x1400) == 0x1000)) &&
                             ((btn & 0x2000) == 0)) {
                            if ((*(ushort *)(ctrl + 0x7c) & 0x400) != 0) {
                              *(uint *)(ctrl + 0x5c) = f68 | 0x10000002;
                            }
                            *(uint *)(ctrl + 0x5c) = *(uint *)(ctrl + 0x5c) | 1;
                          }
                          *(uint *)(ctrl + 0x5c) = *(uint *)(ctrl + 0x5c) | 0x4000;
                          if (cat == 0x6000) {
                            act = 0xd;
                          }
                          else if (cat < 0x6001) {
                            if (cat == 0x2000) {
                              act = 2;
                            }
                            else if (cat == 0x4000) {
                              act = 0xb;
                            }
                          }
                          else if (cat == 0x8000) {
                            if ((f5c & 0x100000) != 0) {
                              *(undefined2 *)(motion_p + 0x82) = 0xffff;
                            }
                            act = 3;
                          }
                          else if (cat == 0xc000) {
                            if ((f5c & 0x100000) != 0) {
                              *(undefined2 *)(motion_p + 0x82) = 0xffff;
                            }
                            act = 0xe;
                          }
                          if ((*(ushort *)(ctrl + 0x7c) & 0x400) != 0) {
                            if (((*(int *)((char *)guest_at((uint32_t)t2) + 0x58) < 0) &&
                                ((int)*(short *)((char *)guest_at((uint32_t)t2) + 0xba) < *(short *)(ctrl + 0x88) * 10))
                               && ((*(short *)((char *)guest_at((uint32_t)t2) + 0xbc) < 1 &&
                                   (*(short *)(ctrl + 0x76) < 0x281)))) {
                              act = 0x3b;
                              if (((btn & 0x4000) == 0) &&
                                 (act = 0x3a, (*(uint *)((char *)guest_at((uint32_t)t2) + 100) & 0x100) != 0)) {
                                act = 0x39;
                              }
                              if ((*(ushort *)(ctrl + 0x7c) & 8) != 0) {
                                return act;
                              }
                            }
                            else if ((*(ushort *)(ctrl + 0x7c) & 8) != 0) {
                              *(undefined2 *)(motion_p + 0x82) = 0xffff;
                              if ((btn & 0x4000) == 0) {
                                return 0x39;
                              }
                              return 0x3b;
                            }
                          }
                          if ((btn & 0x1000) != 0) {
                            return 0x13;
                          }
                          if (cat2 == 0x4000) {
                            act = 0x11;
                          }
                          else if (cat2 < 0x4001) {
                            if (cat2 == 0x2000) {
                              act = 4;
                              *(undefined2 *)(motion_p + 0x98) = 5;
                            }
                          }
                          else if (cat2 == 0x8000) {
                            act = 7;
                          }
                          if ((*(uint *)((char *)guest_at(*(uint32_t *)(motion + 0xc4)) + 8) >> 10 & 1) != 0) {
                            if ((*(ushort *)(ctrl + 0x7a) & 1) != 0) {
                              return 0x1e;
                            }
                            if ((*(ushort *)(ctrl + 0x7a) & 2) != 0) {
                              return 0x1f;
                            }
                          }
                        }
                      }
                      else if ((btn & 0x4000) == 0) {
                        act = 1;
                      }
                    }
                    else {
                      if ((f58 & 8) == 0) {
                        if (cat != 0x2000) {
                          *(undefined2 *)(motion_p + 0x98) = 6;
                        }
                      }
                      else {
                        if ((f58 & 2) != 0) {
                          if (cat != 0x2000) {
                            if (cat < 0x2001) {
                              act = 6;
                              if (cat == 0x1000) {
                                act = 0x1a;
                              }
                            }
                            else if (cat == 0x3000) {
                              act = 0x1b;
                            }
                            else {
                              act = 6;
                              if (cat == 0x9000) {
                                act = 0x1c;
                              }
                            }
                          }
                          if ((f5c & 0x1800000) != 0) {
                            act = 6;
                          }
                        }
                        *(uint *)(ctrl + 0x5c) = *(uint *)(ctrl + 0x5c) | 0x4000;
                      }
                      if ((*(ushort *)(ctrl + 0x7a) & 8) != 0) {
                        act = 0x3c;
                      }
                      if (((*(short *)(motion_p + 0x80) == 6) &&
                          ((*(ushort *)(ctrl + 0x7c) & 0x800) != 0)) &&
                         (((f60 & 0x1000000) == 0 &&
                          (((f5c & 0x1400) == 0x1000 && ((btn & 0x2000) == 0)))))) {
                        if ((*(ushort *)(ctrl + 0x7c) & 0x400) != 0) {
                          *(uint *)(ctrl + 0x5c) = *(uint *)(ctrl + 0x5c) | 2;
                        }
                        f68 = *(uint *)(ctrl + 0x5c);
                        goto LAB_80177a7c;
                      }
                    }
                  }
                  else {
                    cat = *(ushort *)(ctrl + 0xcc);
                    if (*(short *)(ctrl + 0xcc) < 1) {
                      if (*(short *)(ctrl + 0xcc) == 0) goto LAB_80177940;
                    }
                    else {
                      *(short *)(ctrl + 0xcc) =
                           (short)((uint)cat - (uint)*(ushort *)(ctrl + 0x88));
                      if ((int)(((uint)cat - (uint)*(ushort *)(ctrl + 0x88)) * 0x10000) < 0) {
                        ctrl[0xcc] = 0;
                        ctrl[0xcd] = 0;
                      }
                    }
                  }
                }
                else if ((f58 & 0x400000) == 0) {
                  f68 = f58 & 0x2800;
                  if (f68 == 0x800) {
LAB_8017788c:
                    if ((f58 & 0x4000) == 0) {
                      if ((((*(ushort *)(ctrl + 0x7c) & 0x800) != 0) &&
                          ((f60 & 0x1000000) == 0)) &&
                         (((f5c & 0x1400) == 0x1000 && ((btn & 0x2000) == 0)))) {
                        if ((*(ushort *)(ctrl + 0x7c) & 0x400) != 0) {
                          *(uint *)(ctrl + 0x5c) = *(uint *)(ctrl + 0x5c) | 2;
                        }
                        *(uint *)(ctrl + 0x5c) = *(uint *)(ctrl + 0x5c) | 1;
                      }
LAB_80177940:
                      *(uint *)(ctrl + 0x5c) = *(uint *)(ctrl + 0x5c) | 0x4000;
                    }
                  }
                  else if (0x800 < f68) {
                    if (f68 == 0x2000) {
                      if (((((*(ushort *)(ctrl + 0x7c) & 0x800) != 0) &&
                           ((f60 & 0x1000000) == 0)) && ((f5c & 0x1400) == 0x1000)) &&
                         ((btn & 0x2000) == 0)) {
                        if ((*(ushort *)(ctrl + 0x7c) & 0x400) != 0) {
                          *(uint *)(ctrl + 0x5c) = *(uint *)(ctrl + 0x5c) | 2;
                        }
                        *(uint *)(ctrl + 0x5c) = *(uint *)(ctrl + 0x5c) | 1;
                      }
                      goto LAB_80177940;
                    }
                    if (f68 == 0x2800) goto LAB_8017788c;
                  }
                }
              }
            }
            else {
              if ((f58 & 0x8000000) != 0) {
                if ((f5c & 0x80080) != 0x80080) {
                  if (-1 < (int)f5c) {
                    return -1;
                  }
                  if (*(int *)(_DAT_801f04a4 + 2) == 0) {
                    return -1;
                  }
                  act2 = (int)*(short *)(motion_p + 0xd4);
                  t2 = (int)*(short *)(motion_p + 0xd8);
                  if (*_DAT_801f04a4 == 0) {
                    act = _DAT_801f04a4[2] + -0x40;
                    zb = _DAT_801f04a4[3] + -0x40;
                    if (((act < act2) && (0 < *(short *)(ctrl + 0x9a))) ||
                       ((act = -act, act2 < act && (*(short *)(ctrl + 0x9a) < 0)))) {
                      *(short *)(motion_p + 0xd4) = (short)act;
                      ctrl[0xb4] = 0;
                      ctrl[0xb5] = 0;
                      ctrl[0xb2] = 0;
                      ctrl[0xb3] = 0;
                    }
                    if ((zb < t2) && (0 < *(short *)(ctrl + 0x9e))) {
                      *(short *)(motion_p + 0xd8) = (short)zb;
                    }
                    else {
                      if (-zb <= t2) {
                        return -1;
                      }
                      if (-1 < *(short *)(ctrl + 0x9e)) {
                        return -1;
                      }
                      *(short *)(motion_p + 0xd8) = (short)-zb;
                    }
                  }
                  else if (act2 * act2 + t2 * t2 <=
                           *(int *)(_DAT_801f04a4 + 2) + -0x1000) {
                    return -1;
                  }
                  ctrl[0xb4] = 0;
                  ctrl[0xb5] = 0;
                  ctrl[0xb2] = 0;
                  ctrl[0xb3] = 0;
                  return -1;
                }
                if ((f58 & 0x20000) != 0) {
                  act2 = (int)*(short *)(ctrl + 0x70);
                  if (act2 < 0) {
                    if (-act2 < 0x400) {
                      return 0x6d;
                    }
                    return 0x6e;
                  }
                  if (act2 < 0x400) {
                    return 0x6d;
                  }
                  return 0x6e;
                }
                if ((f58 & 0x10000) == 0) {
                  act2 = (int)*(short *)(ctrl + 0x70);
                  if (act2 < 0) {
                    if (-act2 < 0x400) {
                      return 0x6b;
                    }
                    return 0x6c;
                  }
                  if (act2 < 0x400) {
                    return 0x6b;
                  }
                  return 0x6c;
                }
                act2 = (int)*(short *)(ctrl + 0x70);
                if (act2 < 0) {
                  if (-act2 < 0x400) {
                    return 0x70;
                  }
                  return 0x6f;
                }
                if (act2 < 0x400) {
                  return 0x70;
                }
                return 0x6f;
              }
              *(uint *)(ctrl + 0x5c) = *(uint *)(ctrl + 0x5c) | 0x8000;
              cat2 = *(ushort *)(ctrl + 0xc0);
              if (*(short *)(ctrl + 0xc0) < 1) {
                if (*(short *)(ctrl + 0xc0) == 0) {
                  if ((*(ushort *)(ctrl + 0x7c) & 0x200) != 0) {
                    cat = btn & 0xa000;
                    if ((btn & 0x1000) != 0) {
                      cat = cat | 0x4000;
                    }
                    if ((btn & 0x4000) != 0) {
                      cat = cat | 0x1000;
                    }
                  }
                  if ((f58 & 0x20000) == 0) {
                    if ((f6c & 0x10000) == 0) {
                      if (cat == 0x2000) {
                        act = 0x79;
                      }
                      else if (cat < 0x2001) {
                        act = 0x77;
                        if (cat == 0x1000) {
                          act = 0x7c;
                        }
                      }
                      else if (cat == 0x4000) {
                        act = 0x7b;
                      }
                      else {
                        act = 0x77;
                        if (cat == 0x8000) {
                          act = 0x7a;
                        }
                      }
                    }
                    else {
                      act2 = 0x1f;
                    }
                  }
                  else if ((f6c & 0x10000) == 0) {
                    if (cat == 0x2000) {
                      act = 0x73;
                    }
                    else if (cat < 0x2001) {
                      act = 0x71;
                      if (cat == 0x1000) {
                        act = 0x76;
                      }
                    }
                    else if (cat == 0x4000) {
                      act = 0x75;
                    }
                    else {
                      act = 0x71;
                      if (cat == 0x8000) {
                        act = 0x74;
                      }
                    }
                  }
                  else {
                    act2 = 0x1e;
                  }
                }
              }
              else {
                *(short *)(ctrl + 0xc0) =
                     (short)((uint)cat2 - (uint)*(ushort *)(ctrl + 0x88));
                if ((int)(((uint)cat2 - (uint)*(ushort *)(ctrl + 0x88)) * 0x10000) < 0) {
                  ctrl[0xc0] = 0;
                  ctrl[0xc1] = 0;
                }
                if ((*(ushort *)(ctrl + 0x7a) & 0xf0e0) == 0) {
LAB_8017764c:
                  sv = *(short *)(ctrl + 0xc0);
                }
                else {
                  cat = *(ushort *)(ctrl + 0xc0);
                  sv = 0;
                  if (*(short *)(ctrl + 0xc0) != 0) {
                    *(short *)(ctrl + 0xc0) =
                         (short)((uint)cat - (uint)*(ushort *)(ctrl + 0x88));
                    if ((int)(((uint)cat - (uint)*(ushort *)(ctrl + 0x88)) * 0x10000) < 0) {
                      ctrl[0xc0] = 0;
                      ctrl[0xc1] = 0;
                    }
                    goto LAB_8017764c;
                  }
                }
                if ((sv < 0x80) && ((*(ushort *)(ctrl + 0x82) & 0x40) != 0)) {
                  *(uint *)(ctrl + 0x6c) = *(uint *)(ctrl + 0x6c) | 0x10000;
                }
              }
            }
          }
          else {
            if ((f58 & 0x800) == 0) {
              return -1;
            }
            if (0 < *(short *)(ctrl + 0x90)) {
              if (((int)*(short *)(ctrl + 0x94) < *(int *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0x268)) ||
                 ((int)*(short *)(ctrl + 0x94) < *(int *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0x358))) {
                if ((f58 & 0x4000000) != 0) {
                  return (int)*(short *)(motion_p + 0x9a);
                }
                return 0x5a;
              }
            }
            cat = *(ushort *)(ctrl + 0xcc);
            if ((0 < *(short *)(ctrl + 0xcc)) &&
               (*(short *)(ctrl + 0xcc) =
                     (short)((uint)cat - (uint)*(ushort *)(ctrl + 0x88)),
               (int)(((uint)cat - (uint)*(ushort *)(ctrl + 0x88)) * 0x10000) < 0)) {
              ctrl[0xcc] = 0;
              ctrl[0xcd] = 0;
            }
            if ((int)f5c < 0) {
              if (*(int *)(_DAT_801f04a4 + 2) != 0) {
                t2 = *(int *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0x174);
                zb = *(int *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0x17c);
                if (*_DAT_801f04a4 == 0) {
                  if ((((_DAT_801f04a4[2] + -0x40 < t2) && (0 < *(short *)(ctrl + 0x9a))) ||
                      ((t2 < -(_DAT_801f04a4[2] + -0x40) && (*(short *)(ctrl + 0x9a) < 0))))
                     || (((_DAT_801f04a4[3] + -0x40 < zb && (0 < *(short *)(ctrl + 0x9e))) ||
                         ((zb < -(_DAT_801f04a4[3] + -0x40) && (*(short *)(ctrl + 0x9e) < 0)))
                         ))) {
LAB_801770c0:
                    if (*(short *)(ctrl + 0x94) + -0x100 <=
                        *(int *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0x178)) goto LAB_801772e0;
                    act = 0x4c;
                    if (*(short *)(ctrl + 0x96) == 0x800) {
                      act = 0x4b;
                    }
                  }
                }
                else if (*(int *)(_DAT_801f04a4 + 2) + -0x1000 < t2 * t2 + zb * zb)
                goto LAB_801770c0;
              }
            }
            else if ((f58 & 0x4000000) == 0) {
              if (*(short *)(ctrl + 0xb2) == 0) {
                t2 = (int)*(short *)(ctrl + 0x98);
                if (t2 < 0) {
                  t2 = -t2;
                }
                if (t2 < 0x400) goto LAB_801772ec;
              }
              if (((f5c & 0x1000000) != 0) &&
                 (act = 0x8a, *(int *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0x178) < -0x100)) {
LAB_801772c8:
                act = 0x62;
                if (*(short *)(ctrl + 0x96) == 0x800) {
                  act = 0x61;
                }
              }
            }
            else if ((f5c & 0x1000000) != 0) {
              if (((*ctrl & 0x80) == 0) ||
                 (ctrl[0xe5] < *(byte *)((char *)guest_at(*(uint32_t *)(motion + 0xc0)) + 4))) {
                if (*(int *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0x178) < -0x100) {
                  w = (undefined2)
                          ((uint)(((int)*(short *)(ctrl + 0x24) / 10) * 0x10000) >> 0x10);
                  if ((f6c & 1) != 0) {
                    _DAT_801f04f4 = 0xffff;
                    _DAT_801f04ec = w;
                  }
                  if ((f6c & 2) != 0) {
                    _DAT_801f04f6 = 0xffff;
                    _DAT_801f04ee = w;
                  }
                  if ((f6c & 4) != 0) {
                    _DAT_801f04f8 = 0xffff;
                    _DAT_801f04f0 = w;
                  }
                  if ((f6c & 8) != 0) {
                    _DAT_801f04fa = 0xffff;
                    _DAT_801f04f2 = w;
                  }
                  goto LAB_801772c8;
                }
LAB_801772e0:
                act = 0x8a;
              }
              else {
                DAT_801f04c9 = 0;
                if ((f6c & 1) != 0) {
                  _DAT_801f04ec = 10000;
                  _DAT_801f04f4 = 0xffff;
                }
                if ((f6c & 2) != 0) {
                  _DAT_801f04ee = 10000;
                  _DAT_801f04f6 = 0xffff;
                }
                if ((f6c & 4) != 0) {
                  _DAT_801f04f0 = 10000;
                  _DAT_801f04f8 = 0xffff;
                }
                if ((f6c & 8) != 0) {
                  _DAT_801f04f2 = 10000;
                  _DAT_801f04fa = 0xffff;
                }
              }
            }
LAB_801772ec:
            if (((ba & 0xc0) == 0xc0) && (*(short *)(ctrl + 0xcc) == 0)) {
              ctrl[0xc0] = 0;
              ctrl[0xc1] = 0;
              if (*(int *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0x178) - (int)*(short *)(ctrl + 0x94) <
                  -0x98) {
                if ((btn & 0x1000) == 0) {
                  if ((btn & 0x4000) == 0) {
                    ctrl[0x90] = 0;
                    ctrl[0x91] = 0;
                  }
                  else {
                    ctrl[0x90] = 0x80;
                    ctrl[0x91] = 2;
                  }
                }
                else {
                  ctrl[0x90] = 0;
                  ctrl[0x91] = 0xfb;
                }
                act = 0x3d;
                ctrl[0x92] = 0xa0;
                ctrl[0x93] = 0;
                if ((btn & 0x2000) == 0) {
                  if ((btn & 0x8000) == 0) goto LAB_80177350;
                  ctrl[0x98] = 0;
                  ctrl[0x99] = 0xfe;
                }
                else {
                  ctrl[0x98] = 0;
                  ctrl[0x99] = 2;
                }
              }
              else {
                ctrl[0xb4] = 0x78;
                ctrl[0xb5] = 0;
                sv = *(short *)(ctrl + 0x98);
                act = 0x3e;
                if (sv < 0) {
                  sv = -sv;
                }
                *(short *)(ctrl + 0xb2) = *(short *)(ctrl + 0xb2) + sv;
LAB_80177350:
                ctrl[0x98] = 0;
                ctrl[0x99] = 0;
              }
            }
          }
        }
      }
      /* f58 sign bit set: forced/priority state - just set the
       * request-ack bit and fall through to the tail. */
      else if ((int)f58 < 0) {
        f68 = *(uint *)(ctrl + 0x5c) & 0xfdfffffb;
LAB_80177a7c:
        *(uint *)(ctrl + 0x5c) = f68 | 1;
      }
      else {
        if ((f5c & 0x2000000) != 0) {
          *(undefined2 *)(motion_p + 0x80) = 0x30;
          *(undefined2 *)(motion_p + 0x82) = 0xffff;
          ctrl[0xcc] = 0;
          ctrl[0xcd] = 0;
          if ((f58 & 0x800) == 0) {
            if ((f5c & 4) == 0) {
              act = 0x32;
              if (((f58 & 0x10) == 0) && (act = 0x31, (f68 & 0x400000) != 0)) {
                act = 0x30;
              }
            }
            else {
              act = 0x38;
              if (((f58 & 0x10) == 0) && (act = 0x37, (f68 & 0x400000) != 0)) {
                act = 0x36;
              }
            }
          }
          else {
            act = 0x33;
            if ((f5c & 4) != 0) {
              act = 0x34;
            }
          }
        }
        f68 = *(uint *)(ctrl + 0x5c);
        sv = *(short *)(ctrl + 0xcc);
        *(short *)(ctrl + 0xcc) = sv + *(short *)(ctrl + 0x88);
        *(uint *)(ctrl + 0x5c) = f68 & 0xfdfffffb;
        if ((short)(sv + *(short *)(ctrl + 0x88)) < 0x61) {
          if ((f5c & 0x80) == 0) {
            if (((f5c & 0x2004) == 0) && ((*(ushort *)(ctrl + 0x7a) & 0x2000) != 0)) {
              f58 = f68 & 0xfdfffffb | 1;
              t2 = 0x35;
              goto LAB_80176f0c;
            }
          }
          else if (((f5c & 0x2004) == 0) &&
                  (t2 = 0x3f, (*(ushort *)(ctrl + 0x7a) & 0x2000) != 0)) {
            f58 = *(uint *)(ctrl + 0x5c) | 1;
            goto LAB_80176f0c;
          }
        }
        if ((*(short *)(motion_p + 0x80) != 0x35) && (*(short *)(motion_p + 0x80) != 0x3f)) {
          if ((*(ushort *)(ctrl + 0x7c) & 0x800) == 0) {
            return act;
          }
          if ((f60 & 0x1000000) != 0) {
            return act;
          }
          if ((f5c & 0x1400) != 0x1000) {
            return act;
          }
          if ((btn & 0x2000) == 0) {
            if ((*(ushort *)(ctrl + 0x7c) & 0x400) != 0) {
              *(uint *)(ctrl + 0x5c) = *(uint *)(ctrl + 0x5c) | 2;
            }
            *(uint *)(ctrl + 0x5c) = *(uint *)(ctrl + 0x5c) | 1;
            return act;
          }
          return act;
        }
        if ((*(uint *)(ctrl + 0x60) & 0x4000000) != 0) {
          f58 = *(uint *)(ctrl + 0x5c) | 1;
          t2 = act;
LAB_80176f0c:
          *(uint *)(ctrl + 0x5c) = f58;
          return t2;
        }
      }
      if ((f60 & 0x10000) != 0) {
        if (act2 < 0) {
          return act;
        }
        return act2 + 0x90;
      }
      if (0 < *(short *)(ctrl + 0xce)) {
        return act;
      }
      cancel = false;
      if ((((int)f58 < 0) && (*(short *)(ctrl + 0xbc) == *(short *)(ctrl + 0x88))) &&
         (((f64 & 0x80000000) != 0 || ((*(uint *)((char *)guest_at(*(uint32_t *)(motion + 0xc4)) + 8) >> 0xd & 1) != 0)
          ))) {
        cancel = true;
      }
      if (((((f58 & 0x40000) != 0) || ((f58 & 0x40) != 0)) || ((f58 & 0x8000) != 0)) ||
         (((f58 & 0x80080000) == 0x80000 || ((f58 & 0x1000000) != 0)))) goto LAB_8017862c;
      if ((int)f58 < 0) {
        if (cancel) {
          if (((*(ushort *)(ctrl + 0x80) & 0xe8) == 0xe0) && ((ctrl[5] & 0x10) == 0)) {
LAB_8017857c:
            pending_hit_a_clear(ctrl);
            hit_chain_clear(ctrl);
            if (*(short *)(ctrl + 0xd6) < 0x1e00) {
              return 0xbd;
            }
            if (ctrl[1] == 0) {
              if ((DAT_801b55d0 & 1) != 0) {
                (&DAT_801f596c)[(uint)ctrl[2] * 2] = 0x3c;
                (&DAT_801f5970)[(uint)ctrl[2] * 2] = 0xff;
                (&DAT_801f596d)[(uint)ctrl[2] * 2] = 0x3c;
                (&DAT_801f5971)[(uint)ctrl[2] * 2] = 1;
              }
              return 0xbe;
            }
            return 0xbe;
          }
          if (((-1 < *(short *)(motion_p + 0x1c)) && ((*(uint *)(motion_p + 0x18) & 0x20) != 0)) &&
             (act2 = pending_action_gate(guest_of((void *)motion_p),ctrl,f5c,f58), -1 < act2)) {
            return act2;
          }
        }
        f60 = (uint)*(byte *)(motion_p + 0x7c);
        if (((f60 != *(byte *)(motion_p + 0x7d)) && (0 < *(short *)(ctrl + 0xbc))) &&
           (t2 = (int)(guest_of((const void *)motion_p) + f60 * 2),
            *(short *)((char *)guest_at((uint32_t)t2) + 0x5c) <= *(short *)(ctrl + 0xbc)))
        {
          sv = *(short *)((char *)guest_at((uint32_t)t2) + 0x3c);
          if (f60 == (int)*(char *)(motion_p + 0x7e)) {
            *(undefined1 *)(motion_p + 0x7e) = 0xff;
          }
          idx = *(char *)(motion_p + 0x7c) + 1;
          *(byte *)(motion_p + 0x7c) = idx - (idx & 0xf0);
          *(undefined2 *)(motion_p + 0x82) = 0xffff;
          return sv + 0x1090;
        }
        if (((0 < *(short *)(ctrl + 0xba)) && ((f5c & 0x40080) == 0)) &&
           (((f64 & 0x40002000) == 0x40002000 && ((*(ushort *)(ctrl + 0x7a) & 8) != 0)))) {
          if ((f6c & 0x8000) == 0) {
            hit_chain_clear(ctrl);
            pending_hit_a_clear(ctrl);
            pending_hit_b_clear(ctrl);
          }
          else {
            fighter_combat_reset(ctrl);
          }
          if ((f58 & 0x10) == 0) {
            act = 1;
            slot_mark2_set(ctrl[2]);
            *(undefined2 *)(&DAT_801f4d70 + (uint)ctrl[2] * 0xbc) = 0;
            *(undefined2 *)(&DAT_801f4d72 + (uint)ctrl[2] * 0xbc) = 0;
            idx = ctrl[2];
          }
          else {
            act = 0xb;
            slot_mark2_set(ctrl[2]);
            *(undefined2 *)(&DAT_801f4d70 + (uint)ctrl[2] * 0xbc) = 0;
            *(undefined2 *)(&DAT_801f4d72 + (uint)ctrl[2] * 0xbc) = 0;
            idx = ctrl[2];
          }
          *(undefined2 *)(&DAT_801f4d76 + (uint)idx * 0xbc) = 0xffe0;
          ctrl[0xce] = 0xa0;
          ctrl[0xcf] = 0;
        }
        goto LAB_8017862c;
      }
      if ((f58 & 0x1000) == 0) {
        if ((f58 & 0x400) == 0) {
          if ((f58 & 0x20) != 0) {
            if ((ba & 0x80) == 0) {
              if ((ba & 0x40) == 0) {
                if ((ba & 0x20) != 0) {
                  act2 = 0x2a;
                }
              }
              else {
                act2 = 0x1a;
              }
            }
            else {
              act2 = 10;
            }
            goto LAB_8017862c;
          }
        }
        else {
          if ((f58 & 8) == 0) {
            if ((f58 & 2) != 0) {
              if ((ba & 0x80) == 0) {
                if ((ba & 0x40) == 0) {
                  if ((ba & 0x20) != 0) {
                    act2 = 0x26;
                  }
                }
                else {
                  act2 = 0x16;
                }
              }
              else {
                act2 = 6;
              }
            }
            goto LAB_8017862c;
          }
          if (((f58 & 2) != 0) && ((btn & 0x2000) != 0)) {
            if ((ba & 0x80) == 0) {
              if ((ba & 0x40) == 0) {
                if (((*(ushort *)(ctrl + 0x80) & 0x20) != 0) &&
                   (((ctrl[5] & 0x10) != 0 || (0x1dff < *(short *)(ctrl + 0xd6))))) {
                  act2 = 0x27;
                }
              }
              else {
                act2 = 0x17;
              }
            }
            else {
              act2 = 7;
            }
            goto LAB_8017862c;
          }
        }
        if ((-1 < *(short *)(motion_p + 0x1c)) &&
           (act2 = pending_action_gate(guest_of((void *)motion_p),ctrl,f5c,f58), -1 < act2)) {
          return act2;
        }
        if ((btn & 0x4000) == 0) {
          if ((f5c & 0x400000) == 0) {
            if ((btn & 0x2000) == 0) {
              if ((btn & 0x8000) == 0) {
                if ((ba & 0x80) == 0) {
                  if ((ba & 0x40) == 0) {
                    if ((ba & 0x20) != 0) {
                      act2 = 0x20;
                    }
                  }
                  else {
                    act2 = 0x10;
                  }
                }
                else {
                  act2 = 0;
                }
              }
              else if ((ba & 0x80) == 0) {
                if ((ba & 0x40) == 0) {
                  if ((ba & 0x20) != 0) {
                    act2 = 0x22;
                  }
                }
                else {
                  act2 = 0x12;
                }
              }
              else {
                act2 = 2;
              }
            }
            else if ((ba & 0x80) == 0) {
              if ((ba & 0x40) == 0) {
                if ((ba & 0x20) != 0) {
                  act2 = 0x21;
                }
              }
              else {
                act2 = 0x11;
              }
            }
            else {
              act2 = 1;
            }
          }
          else if ((ba & 0x80) == 0) {
            if ((ba & 0x40) == 0) {
              if ((ba & 0x20) != 0) {
                act2 = 0x2b;
              }
            }
            else {
              act2 = 0x1b;
            }
          }
          else {
            act2 = 0xb;
          }
        }
        else {
          if ((f5c & 0x400000) == 0) {
            if ((btn & 0x2000) == 0) {
              if ((btn & 0x8000) == 0) {
                if ((ba & 0x80) == 0) {
                  if ((ba & 0x40) == 0) {
                    if ((ba & 0x20) != 0) {
                      act2 = 0x23;
                    }
                  }
                  else {
                    act2 = 0x13;
                  }
                }
                else {
                  act2 = 3;
                }
              }
              else if ((ba & 0x80) == 0) {
                if ((ba & 0x40) == 0) {
                  if ((ba & 0x20) != 0) {
                    act2 = 0x25;
                  }
                }
                else {
                  act2 = 0x15;
                }
              }
              else {
                act2 = 5;
              }
            }
            else if ((ba & 0x80) == 0) {
              if ((ba & 0x40) == 0) {
                if ((ba & 0x20) != 0) {
                  act2 = 0x24;
                }
              }
              else {
                act2 = 0x14;
              }
            }
            else {
              act2 = 4;
            }
          }
          else if ((ba & 0x80) == 0) {
            if ((ba & 0x40) == 0) {
              if ((ba & 0x20) != 0) {
                act2 = 0x2c;
              }
            }
            else {
              act2 = 0x1c;
            }
          }
          else {
            act2 = 0xc;
          }
          if (((f5c & 0x800) != 0) && ((*(ushort *)(ctrl + 0x80) & 0x4000) != 0)) {
            if ((ba & 0x80) == 0) {
              if ((ba & 0x40) == 0) {
                if ((ba & 0x20) != 0) {
                  act2 = 0x32;
                }
              }
              else {
                act2 = 0x31;
              }
            }
            else {
              act2 = 0x30;
            }
          }
        }
        if (((*(ushort *)(ctrl + 0x80) & 0x20) != 0) && ((ctrl[5] & 0x10) == 0))
        goto LAB_8017857c;
        goto LAB_8017862c;
      }
      if ((f58 & 0x400000) != 0) goto LAB_8017862c;
      if ((*(uint *)(motion_p + 0x30) & 0x20000) == 0) {
        if ((*(uint *)(motion_p + 0x30) & 0x40000) == 0) {
          if ((*(int *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0x178) < (int)*(short *)(motion_p + 0x38)) &&
             ((f58 & 0x800) != 0)) goto LAB_80178118;
        }
        else if ((f58 & 0x2800) == 0x2000) goto LAB_80178120;
      }
      else if ((f58 & 0x2800) == 0) {
LAB_80178118:
        act2 = (int)*(short *)(motion_p + 0x34);
LAB_80178120:
        *(undefined2 *)(motion_p + 0x34) = 0xffff;
      }
      f58 = f58 & 0x2800;
      if ((((f58 == 0x800) || (((0x800 < f58 && (f58 != 0x2000)) && (f58 == 0x2800))))
          && ((((*(ushort *)(ctrl + 0x80) & 0x20) != 0 && (0x1dff < *(short *)(ctrl + 0xd6)))
              && ((ctrl[5] & 0x10) == 0)))) &&
         ((act2 = 0x2e, ctrl[1] == 0 && ((DAT_801b55d0 & 1) != 0)))) {
        (&DAT_801f596c)[(uint)ctrl[2] * 2] = 0x3c;
        (&DAT_801f5970)[(uint)ctrl[2] * 2] = 0xff;
        (&DAT_801f596d)[(uint)ctrl[2] * 2] = 0x3c;
        (&DAT_801f5971)[(uint)ctrl[2] * 2] = 1;
      }
      /* attack/cancel selection tail: act2<0 keeps act, else
       * returns act2+0x90 (extended action range). */
LAB_8017862c:
      if (act2 < 0) {
        return act;
      }
      return act2 + 0x90;
    }
    sv = *(short *)(ctrl + 0xd4);
    if ((*(short *)(ctrl + 0xd4) != 0) &&
       (*(ushort *)(ctrl + 0xd4) = sv - 0x10U,
       (int)((uint)(ushort)(sv - 0x10U) << 0x10) < 0)) {
      ctrl[0xd4] = 0;
      ctrl[0xd5] = 0;
    }
    if ((f6c & 0x4000000) != 0) {
      *(uint *)(ctrl + 0x6c) = *(uint *)(ctrl + 0x6c) & 0xfbffffff;
      *(short *)(ctrl + 0xce) = *(short *)(ctrl + 0x88) * 0x26;
      pending_hit_a_clear(ctrl);
      pending_hit_b_clear(ctrl);
      return 0x7e;
    }
    if ((f6c & 0x80000) == 0) goto LAB_801766c8;
  }
  /* f58 high bit set: special-state tail - juggle timer, then
   * get-up path or -1. */
  else {
    if ((f6c & 0x4000000) != 0) {
      *(uint *)(ctrl + 0x6c) = *(uint *)(ctrl + 0x6c) & 0xfbffffff;
      *(short *)(ctrl + 0xce) = *(short *)(ctrl + 0x88) * 0x26;
      pending_hit_a_clear(ctrl);
      pending_hit_b_clear(ctrl);
      return 0x7f;
    }
    if ((f6c & 0x80000) == 0) {
      if ((f5c & 0x4000000) == 0) {
LAB_801766c8:
        *(uint *)(ctrl + 0x5c) = *(uint *)(ctrl + 0x5c) & 0xfbffffff;
        return -1;
      }
      /* get-up/knockdown path: clear flags, mark slot, reset or
       * clear hits, then pick directional get-up action (0x30-0x5b
       * range) from f68/f58 nibble combos. */
LAB_801766e8:
      *(uint *)(ctrl + 0x5c) = *(uint *)(ctrl + 0x5c) & 0xfbffffff;
      *(uint *)(ctrl + 0x6c) = *(uint *)(ctrl + 0x6c) & 0xfff7efff;
      *(undefined2 *)(motion_p + 0x82) = 0xffff;
      slot_mark2_set(ctrl[2]);
      if ((f6c & 0x8000) == 0) {
        pending_hit_a_clear(ctrl);
      }
      else {
        fighter_combat_reset(ctrl);
      }
      *(undefined2 *)(&DAT_801f4d70 + (uint)ctrl[2] * 0xbc) = 0;
      *(undefined2 *)(&DAT_801f4d72 + (uint)ctrl[2] * 0xbc) = 0;
      *(undefined2 *)(&DAT_801f4d76 + (uint)ctrl[2] * 0xbc) = 0xff00;
      if ((f68 & 0x2000000) != 0) {
        if ((f58 & 0x20000) != 0) {
          return 0x69;
        }
        if ((f58 & 0x10000) != 0) {
          return 0x6a;
        }
      }
      if ((*ctrl & 0x80) != 0) {
        *(uint *)(ctrl + 0x5c) = *(uint *)(ctrl + 0x5c) & 0xffdfffff;
        if (((f68 & 0x4000) == 0) && ((f68 & 0x800) == 0)) {
          if ((f68 & 0x10000) == 0) {
            if ((f68 & 0x2000) == 0) {
              if ((f68 & 0x40000) == 0) {
                *(uint *)(ctrl + 0x68) = *(uint *)(ctrl + 0x68) & 0xffffefff | 0x804400;
                trig_lut_read(t2,ctrl);
                if ((ctrl[5] & 1) == 0) {
                  return 0x5b;
                }
                if ((ctrl[0xe5] < *(byte *)((char *)guest_at(*(uint32_t *)(motion + 0xc0)) + 4)) &&
                   (-1 < *(short *)(ctrl + 0xd6))) {
                  return 0x5b;
                }
                fighter_combat_reset(ctrl);
                return 0x5b;
              }
              f68 = f68 | 0x800000;
              *(uint *)(ctrl + 0x68) = f68;
              trig_vel_diag(t2,ctrl);
            }
            else {
              f68 = f68 | 0x800000;
              *(uint *)(ctrl + 0x68) = f68;
              trig_vel_up(t2,ctrl);
            }
          }
          else {
            f68 = f68 | 0x800000;
            *(uint *)(ctrl + 0x68) = f68;
            trig_vel_flat(t2,ctrl);
          }
        }
        else {
          f68 = f68 | 0x800000;
          *(uint *)(ctrl + 0x68) = f68;
          trig_lut_read(t2,ctrl);
        }
      }
      if ((((f68 & 0x2000000) != 0) ||
          ((((f68 & 0x800000) != 0 && ((f68 & 0x56800) != 0)) || ((f68 & 0x2000) != 0))))
         && (((ctrl[5] & 1) != 0 && (*(short *)(ctrl + 0xd6) < 0)))) {
        fighter_combat_reset(ctrl);
      }
      if (((f68 & 3) == 2) && ((bb & 0x400) == 0)) {
        act = 0x45;
      }
      else if ((f58 & 0x30) == 0) {
        if ((f68 & 0x100000) == 0) {
          if ((f68 & 0x200000) == 0) {
            if ((f68 & 0x400000) != 0) {
              act = 0x40;
            }
          }
          else {
            act = 0x41;
          }
        }
        else {
          act = 0x42;
        }
      }
      else if ((f68 & 0x100000) == 0) {
        if ((f68 & 0x600000) != 0) {
          act = 0x43;
        }
      }
      else {
        act = 0x44;
      }
      if ((f68 & 0x800) == 0) goto LAB_80176a88;
      f58 = f68 & 0xc;
      if ((f5c & 0x100000) == 0) {
        if (f58 != 4) {
          if (f58 != 8) goto LAB_80176a88;
          goto LAB_80176a80;
        }
      }
      else {
        if (f58 == 4) {
LAB_80176a80:
          act = 0x58;
          goto LAB_80176a88;
        }
        if (f58 != 8) goto LAB_80176a88;
      }
      act = 0x59;
LAB_80176a88:
      if (((f68 & 0x1000) != 0) && (act = 0x4f, (f68 & 3) == 2)) {
        act = 0x53;
      }
      if ((f68 & 0x2000) != 0) {
        if ((f68 & 0x800000) == 0) {
          if ((f68 & 0xc) == 4) {
            act = 0x84;
          }
          else if ((f68 & 0xc) == 8) {
            act = 0x85;
          }
          if ((f68 & 3) == 1) {
            act = 0x88;
          }
          else if ((f68 & 3) == 2) {
            act = 0x89;
          }
        }
        else {
          act = 0x50;
          if ((f68 & 3) == 2) {
            act = 0x51;
          }
        }
      }
      if ((((f68 & 0x4000) != 0) && (act = 0x5b, (f68 & 0x800000) == 0)) &&
         (act = 0x80, (f68 & 3) == 2)) {
        act = 0x82;
      }
      if ((f68 & 0x8000) != 0) {
        if ((f68 & 0xc) == 4) {
          act = 0x5f;
        }
        else if ((f68 & 0xc) == 8) {
          act = 0x5e;
        }
        if ((f5c & 0x100000) != 0) {
          act = 0x53;
        }
      }
      if ((f68 & 0x10000) != 0) {
        f58 = f68 & 0xc;
        if ((f68 & 0x800000) == 0) {
          if (f58 == 4) {
            act = 0x87;
          }
          else if (f58 == 8) {
            act = 0x86;
          }
          if ((f68 & 3) == 1) {
            act = 0x8c;
          }
          else if ((f68 & 3) == 2) {
            act = 0x8d;
          }
        }
        else {
          if (f58 == 4) {
            act = 0x55;
          }
          else if (f58 == 8) {
            act = 0x56;
          }
          if ((f68 & 3) == 1) {
            act = 0x60;
          }
        }
      }
      if ((f68 & 0x40000) == 0) {
        return act;
      }
      if ((f68 & 0x800000) == 0) {
        if ((f68 & 3) != 2) {
          return 0x81;
        }
        return 0x83;
      }
      if ((f68 & 3) != 2) {
        return 0x54;
      }
      return 0x57;
    }
  }
LAB_80176d18:
  f60 = *(uint *)(ctrl + 0x6c);
LAB_80176d24:
  *(uint *)(ctrl + 0x6c) = f60 & 0xfff7ffff;
  return (int)*(short *)(motion_p + 0x9a);
}


/* FUN_801787dc — per-fighter frame update. Calls the resolver and
 * applies its result to motion+0x80, then physics/anim tail. */
extern uint8_t *g_DAT_801b4e2c, *g_DAT_801b4e38, *g_DAT_801b4e44,
              *g_DAT_801b4e50, *g_DAT_801b4e5c;
#define DAT_801b4e2c   (*g_DAT_801b4e2c)
#define DAT_801b4e38   (*g_DAT_801b4e38)
#define DAT_801b4e44   (*g_DAT_801b4e44)
#define DAT_801b4e50   (*g_DAT_801b4e50)
#define DAT_801b4e5c   (*g_DAT_801b4e5c)
extern void motion_turn_update(byte*,byte*);
int range_circle_test(int,int,uint*,int);
void deferred_cmd_post(uint,int,short);
extern void action_commit(byte*,byte*);

extern void launch_vel_update(byte*,byte*);
extern void ev_stream_dispatch(byte *ctrl,byte *motion);
extern void action_switch_guest(int ctrl,int *motion,int act);
extern void slot_apply_guest(int ctrl,int motion);
extern void part_record_reset(undefined2 *ctrl);
extern ushort * ev5_action_select(int ctrl,int motion,ushort *stream);
void part_matrix_compute(undefined4 *mat,byte *parts,short *euler,undefined4 angle);
uint * matrix_vec3_apply(undefined4 *mat,uint *vec);
extern uint8_t *g_DAT_1f8003a0,*g_DAT_1f8003c0;
#define DAT_1f8003a0 (*g_DAT_1f8003a0)
#define DAT_1f8003c0 (*g_DAT_1f8003c0)
typedef uint8_t *evhandler_t(uint8_t *ctrl, uint8_t *motion, uint32_t pc_g);
extern evhandler_t *PTR_FUN_801b4e68[];
extern void FUN_801851d8();
extern int trig_cos();
extern int trig_sin();
extern void gte_mat_vec_swap();
extern void setCopReg(int,int,uint);
extern int getCopReg(int,int);
extern void copFunction(int,int);
extern void setCopControlWord(int,int,uint);
extern uint32_t *g__DAT_1f8003a0,*g__DAT_1f8003a4,*g__DAT_1f8003a8,*g__DAT_1f8003ac,
  *g__DAT_1f8003b0,*g__DAT_1f8003b4,*g__DAT_1f8003b8,*g__DAT_1f8003bc,
  *g__DAT_1f8003c0,*g__DAT_1f8003c4,*g__DAT_1f8003c8,*g__DAT_1f8003cc,
  *g__DAT_1f8003d0,*g__DAT_1f8003d4,*g__DAT_1f8003d8,*g__DAT_1f8003dc,
  *g__DAT_1f8003e0,*g__DAT_1f8003e4,*g__DAT_1f8003e8,*g__DAT_1f8003ec,
  *g__DAT_1f8003f0,*g__DAT_1f8003f4,*g__DAT_1f8003f8,*g__DAT_1f8003fc;
#define _DAT_1f8003a0 (*g__DAT_1f8003a0)
#define _DAT_1f8003a4 (*g__DAT_1f8003a4)
#define _DAT_1f8003a8 (*g__DAT_1f8003a8)
#define _DAT_1f8003ac (*g__DAT_1f8003ac)
#define _DAT_1f8003b0 (*(int16_t *)g__DAT_1f8003b0) /* guest lh/sh @0x1f8003b0 (0x80185048) */
#define _DAT_1f8003b4 (*g__DAT_1f8003b4)
#define _DAT_1f8003b8 (*g__DAT_1f8003b8)
#define _DAT_1f8003bc (*g__DAT_1f8003bc)
#define _DAT_1f8003c0 (*(int16_t *)g__DAT_1f8003c0) /* guest sh @0x1f8003c0 (0x80185048) */
#define _DAT_1f8003c4 (*(int16_t *)g__DAT_1f8003c4) /* guest sh @0x1f8003c4 */
#define _DAT_1f8003c8 (*(int16_t *)g__DAT_1f8003c8) /* guest sh @0x1f8003c8 */
#define _DAT_1f8003cc (*(int16_t *)g__DAT_1f8003cc) /* guest sh @0x1f8003cc */
#define _DAT_1f8003d0 (*(int16_t *)g__DAT_1f8003d0) /* guest sh @0x1f8003d0 */
#define _DAT_1f8003d4 (*g__DAT_1f8003d4)
#define _DAT_1f8003d8 (*g__DAT_1f8003d8)
#define _DAT_1f8003dc (*g__DAT_1f8003dc)
#define _DAT_1f8003e0 (*g__DAT_1f8003e0)
#define _DAT_1f8003e4 (*g__DAT_1f8003e4)
#define _DAT_1f8003e8 (*g__DAT_1f8003e8)
#define _DAT_1f8003ec (*g__DAT_1f8003ec)
#define _DAT_1f8003f0 (*g__DAT_1f8003f0)
#define _DAT_1f8003f4 (*g__DAT_1f8003f4)
#define _DAT_1f8003f8 (*g__DAT_1f8003f8)
#define _DAT_1f8003fc (*g__DAT_1f8003fc)
#define _DAT_1f8003a2 (*(uint16_t *)&_DAT_1f8003a0)
#define _DAT_1f8003a6 (*(uint16_t *)&_DAT_1f8003a4)
#define _DAT_1f8003aa (*(uint16_t *)&_DAT_1f8003a8)
#define _DAT_1f8003ae (*(uint16_t *)&_DAT_1f8003ac)
#define _DAT_1f8003b2 (*(uint16_t *)&_DAT_1f8003b0)
#define _DAT_1f8003b6 (*(uint16_t *)&_DAT_1f8003b4)
#define _DAT_1f8003ba (*(uint16_t *)&_DAT_1f8003b8)
#define _DAT_1f8003be (*(uint16_t *)&_DAT_1f8003bc)
#define _DAT_1f8003c2 (*(uint16_t *)&_DAT_1f8003c0)
#define _DAT_1f8003c6 (*(uint16_t *)&_DAT_1f8003c4)
#define _DAT_1f8003ca (*(uint16_t *)&_DAT_1f8003c8)
#define _DAT_1f8003ce (*(uint16_t *)&_DAT_1f8003cc)
#define _DAT_1f8003d2 (*(uint16_t *)&_DAT_1f8003d0)
#define _DAT_1f8003d6 (*(uint16_t *)&_DAT_1f8003d4)
#define _DAT_1f8003da (*(uint16_t *)&_DAT_1f8003d8)
#define _DAT_1f8003de (*(uint16_t *)&_DAT_1f8003dc)

/* fighter_frame - per-fighter frame update: resource drain,
 * floor/height clamps, hit-zone tests, turn, action resolve+commit. */
void fighter_frame(byte *ctrl, uint8_t *motion)
{

  byte fidx;
  bool downed;
  short dv;
  ushort acc;
  int iv3;
  uint uv;
  uint z2;
  uint z3;
  undefined *tab;
  int iv;
  undefined4 auxv;
  int iv2;
  byte *motion_p;
  
  iv = *(char *)(motion + 0x98) * 8;
  iv2 = iv + -0x7fe4eadc;
  motion_p = (byte *)guest_at(*(uint32_t *)(ctrl + 0x38));
  downed = false;
  if (ctrl[1] == '\0') {
    dv = *(short *)(ctrl + 200);
    iv3 = (int)dv;
    if (iv3 < 0) {
      if ((DAT_801b55d0 & 1) != 0) {
        (&DAT_801f596c)[(uint)(byte)ctrl[2] * 2] = 5;
        (&DAT_801f5970)[(uint)(byte)ctrl[2] * 2] = '@' - (char)dv;
      }
    }
    else if (iv3 != 0) {
      iv3 = iv3 * 4 + 0x40;
      if (0xff < iv3) {
        iv3 = 0xff;
      }
      if ((DAT_801b55d0 & 1) != 0) {
        (&DAT_801f596c)[(uint)(byte)ctrl[2] * 2] = 0xf;
        (&DAT_801f5970)[(uint)(byte)ctrl[2] * 2] = (char)iv3;
        (&DAT_801f596d)[(uint)(byte)ctrl[2] * 2] = 0xf;
        (&DAT_801f5971)[(uint)(byte)ctrl[2] * 2] = 1;
      }
    }
    if ((*(short *)(ctrl + 0xb0) < *(short *)(ctrl + 0xac)) && ((DAT_801b55d0 & 1) != 0)) {
      (&DAT_801f596c)[(uint)(byte)ctrl[2] * 2] = 0x78;
      (&DAT_801f5970)[(uint)(byte)ctrl[2] * 2] = 0xff;
      (&DAT_801f596d)[(uint)(byte)ctrl[2] * 2] = 0x78;
      (&DAT_801f5971)[(uint)(byte)ctrl[2] * 2] = 1;
    }
  }
  if (((*(short *)(ctrl + 0xac) < 0) || ((*(uint *)(ctrl + 0x60) & 0x800000) != 0)) ||
     (*ctrl != '\x01')) {
    ctrl[200] = '\0';
    ctrl[0xc9] = '\0';
  }
  else {
    if ((*(uint *)(ctrl + 0x54) & 2) == 0) {
      dv = *(short *)(ctrl + 0xd8);
      if (dv < 0) {
        if (-1 < *(short *)(ctrl + 0xd6)) {
          *(short *)(ctrl + 0xd6) = *(short *)(ctrl + 0xd6) + *(short *)(ctrl + 0xd8);
          ctrl[0xd8] = '\0';
          ctrl[0xd9] = '\0';
          if (*(short *)(ctrl + 0xd6) < 0) {
            ctrl[0xd6] = -1;
            ctrl[0xd7] = -1;
          }
        }
      }
      else if (0 < dv) {
        if (*(short *)(ctrl + 0xd6) < 0) {
          ctrl[0xd8] = '\0';
          ctrl[0xd9] = '\0';
        }
        else {
          *(ushort *)(ctrl + 0xd6) =
               *(short *)(ctrl + 0xd6) + dv * (ushort)*(byte *)((char *)guest_at(*(uint32_t *)(motion + 0xc4)) + 3);
          ctrl[0xd8] = '\0';
          ctrl[0xd9] = '\0';
          if (0x3bff < *(short *)(ctrl + 0xd6)) {
            ctrl[0xd6] = -1;
            ctrl[0xd7] = ';';
          }
        }
      }
      if ((ctrl[5] & 0x10U) == 0) {
        if ((*(short *)(ctrl + 0xd6) < 0) || ((*(uint *)(ctrl + 0x6c) & 0x8000) != 0)) {
          if ((*(uint *)(ctrl + 0x58) & 0x8000) == 0) {
            ctrl[0xd6] = '\0';
            ctrl[0xd7] = '\0';
          }
        }
        else {
          dv = *(short *)(ctrl + 0xd6) + (ushort)*(byte *)((char *)guest_at(*(uint32_t *)(motion + 0xc4)) + 3);
          *(short *)(ctrl + 0xd6) = dv;
          if (0x3bff < dv) {
            ctrl[0xd6] = -1;
            ctrl[0xd7] = ';';
          }
        }
      }
      if (*(short *)(ctrl + 0xaa) < *(short *)(ctrl + 0xae)) {
        *(short *)(ctrl + 0xaa) = *(short *)(ctrl + 0xaa) + 1;
      }
    }
    if ((((ctrl[5] & 0x10U) != 0) && ((*(uint *)(ctrl + 0x58) & 0x10008000) == 0)) &&
       (*(short *)(ctrl + 0xae) < *(short *)(ctrl + 0xac))) {
      dv = *(short *)(ctrl + 0xfa);
      *(short *)(ctrl + 0xfa) = dv + *(short *)(ctrl + 0xf8);
      iv3 = (int)(short)(dv + *(short *)(ctrl + 0xf8));
      if (0xff < iv3) {
        if (iv3 < 0) {
          iv3 = iv3 + 0xff;
        }
        iv3 = (uint)*(ushort *)(ctrl + 0xac) - (iv3 >> 8);
        *(short *)(ctrl + 0xac) = (short)iv3;
        if (iv3 * 0x10000 >> 0x10 < (int)*(short *)(ctrl + 0xae)) {
          *(undefined2 *)(ctrl + 0xac) = *(undefined2 *)(ctrl + 0xae);
        }
        *(ushort *)(ctrl + 0xfa) = (ushort)(byte)ctrl[0xfa];
      }
    }
    dv = *(short *)(ctrl + 200);
    if (*(short *)(ctrl + 200) < 0) {
      if ((*(uint *)(ctrl + 0x54) & 2) == 0) {
        *(short *)(ctrl + 0xac) = *(short *)(ctrl + 0xac) - dv;
        *(short *)(ctrl + 0xae) =
             *(short *)(ctrl + 0xae) - (short)((*(short *)(ctrl + 200) * 0x3c) / 100);
      }
      else {
        *(short *)(ctrl + 0xaa) = *(short *)(ctrl + 0xaa) - dv;
      }
      if (*(short *)(ctrl + 0xb0) < *(short *)(ctrl + 0xae)) {
        *(undefined2 *)(ctrl + 0xae) = *(undefined2 *)(ctrl + 0xb0);
      }
      if (*(short *)(ctrl + 0xb0) < *(short *)(ctrl + 0xac)) {
        *(undefined2 *)(ctrl + 0xac) = *(undefined2 *)(ctrl + 0xb0);
      }
      dv = *(short *)(ctrl + 200);
      ctrl[200] = '\0';
      ctrl[0xc9] = '\0';
      dv = -dv;
LAB_80178d48:
      *(short *)(ctrl + 0xca) = dv;
    }
    else if (*(short *)(ctrl + 200) != 0) {
      if ((*(uint *)(ctrl + 0x54) & 2) == 0) {
        *(short *)(ctrl + 0xac) = *(short *)(ctrl + 0xac) + dv;
        *(short *)(ctrl + 0xae) =
             *(short *)(ctrl + 0xae) + (short)((*(short *)(ctrl + 200) * 0x3c) / 100);
      }
      else {
        *(short *)(ctrl + 0xaa) = *(short *)(ctrl + 0xaa) + dv;
      }
      if (*(short *)(ctrl + 0xb0) < *(short *)(ctrl + 0xae)) {
        *(undefined2 *)(ctrl + 0xae) = *(undefined2 *)(ctrl + 0xb0);
      }
      dv = *(short *)(ctrl + 200);
      ctrl[200] = '\0';
      ctrl[0xc9] = '\0';
      goto LAB_80178d48;
    }
    if ((((ctrl[5] & 0x10U) != 0) && ((*(uint *)(ctrl + 0x5c) & 0x60000000) == 0)) &&
       ((*(uint *)(ctrl + 0x58) & 0x10008000) == 0)) {
      ctrl[0xca] = '\0';
      ctrl[0xcb] = '\0';
      ctrl[200] = '\0';
      ctrl[0xc9] = '\0';
    }
    if (*(short *)(ctrl + 0xb0) < *(short *)(ctrl + 0xac)) {
      downed = true;
      *ctrl = -0x80;
      ctrl[0xac] = -1;
      ctrl[0xad] = -1;
      ctrl[0xae] = -1;
      ctrl[0xaf] = -1;
      ctrl[0xc0] = -1;
      ctrl[0xc1] = -1;
      *(uint *)(ctrl + 0x5c) = *(uint *)(ctrl + 0x5c) | 0x200000;
      ctrl[0xe5] = ctrl[0xe5] + '\x01';
    }
  }
  motion_turn_update(ctrl,motion_p);
  if (((*(uint *)(ctrl + 0x60) & 0x10000000) == 0) &&
     ((*(uint *)(ctrl + 0x6c) & 0x100000) == 0)) {
    dv = range_circle_test(*(undefined4 *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0x174),
                         *(undefined4 *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0x17c),(uint *)guest_at((uint32_t)(iv + -0x7fe4ea74)),0);
    *(short *)(ctrl + 0x94) = dv;
    if (dv == 0) {
LAB_80178f9c:
      ctrl[0x94] = '\0';
      ctrl[0x95] = '\0';
      uv = *(uint *)(ctrl + 0x60) & 0xdfffffff;
    }
    else {
      uv = range_circle_test(*(undefined4 *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0xd4),
                           *(undefined4 *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0xdc),(uint *)guest_at((uint32_t)iv2),0);
      z2 = range_circle_test(*(undefined4 *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0x354),
                           *(undefined4 *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0x35c),(uint *)guest_at((uint32_t)iv2),0);
      z3 = range_circle_test(*(undefined4 *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0x264),
                           *(undefined4 *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0x26c),(uint *)guest_at((uint32_t)iv2),0);
      if ((uv == 0 && z2 == 0) && z3 == 0) goto LAB_80178f9c;
      if ((-1 < *(int *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0x38)) &&
         ((*(uint *)(ctrl + 0x6c) & 0x100000) == 0)) {
        if (((uv & z2 & z3) == 0) && ((*(uint *)(ctrl + 0x58) & 0x1000000) == 0)) {
          if ((*(uint *)(ctrl + 0x5c) & 0x80) != 0) {
            *(uint *)(ctrl + 0x6c) = *(uint *)(ctrl + 0x6c) | 0x80000;
          }
          *(uint *)(ctrl + 0x6c) = *(uint *)(ctrl + 0x6c) | 0x100000;
        }
        if (((*(uint *)(ctrl + 0x5c) & 0x80) == 0) ||
           (0x1ff < *(int *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0x38))) {
          if (*ctrl == '\x01') {
            ctrl[0xc0] = -1;
            ctrl[0xc1] = -1;
            *ctrl = -0x7f;
          }
          ctrl[0xac] = -1;
          ctrl[0xad] = -1;
          ctrl[0xae] = -1;
          ctrl[0xaf] = -1;
          *(uint *)(ctrl + 0x5c) = *(uint *)(ctrl + 0x5c) | 0x80000;
        }
      }
      uv = *(uint *)(ctrl + 0x60) | 0x20000000;
    }
    *(uint *)(ctrl + 0x60) = uv;
  }
  if (*(short *)(ctrl + 0x94) == 0) {
    uv = *(uint *)(ctrl + 0x5c) & 0x7fffffff;
  }
  else {
    uv = *(uint *)(ctrl + 0x5c) | 0x80000000;
  }
  *(uint *)(ctrl + 0x5c) = uv;
  uv = *(uint *)(ctrl + 0x5c);
  iv2 = *(int *)(ctrl + 0x20);
  *(uint *)(ctrl + 0x5c) = uv & 0xeffba5fc;
  iv = *(short *)(ctrl + 0x94) + -0x66;
  if (((*(int *)((char *)guest_at((uint32_t)iv2) + 0x268) < iv) && (*(int *)((char *)guest_at((uint32_t)iv2) + 0x358) < iv)) &&
     (*(int *)((char *)guest_at((uint32_t)iv2) + 0x178) < iv)) {
    *(uint *)(ctrl + 0x5c) = uv & 0xeffba5fc | 0x40000;
  }
  if (*(int *)((char *)guest_at(*(uint32_t *)(ctrl + 0x2c)) + 0x58) < 0) {
    *(uint *)(ctrl + 0x5c) = *(uint *)(ctrl + 0x5c) | 0x1000;
  }
  if (((*(uint *)((char *)guest_at(*(uint32_t *)(ctrl + 0x2c)) + 0x58) & 0x30000) != 0) &&
     ((*(uint *)((char *)guest_at(*(uint32_t *)(ctrl + 0x2c)) + 0x58) & 0x40800) == 0)) {
    *(uint *)(ctrl + 0x5c) = *(uint *)(ctrl + 0x5c) | 0xa00;
  }
  if ((*(uint *)((char *)guest_at(*(uint32_t *)(ctrl + 0x2c)) + 0x58) & 0x8040000) == 0x8000000) {
    *(uint *)(ctrl + 0x5c) = *(uint *)(ctrl + 0x5c) | 0x200;
  }
  if (downed) {
    if ((ctrl[5] & 1U) == 0) {
      fidx = ctrl[3];
      tab = &DAT_801b4e5c;
    }
    else {
      fidx = ctrl[3];
      tab = &DAT_801b4e50;
    }
    uv = (uint)(byte)tab[fidx];
    if ((uv < 0xc0) || (auxv = 5, ctrl[2] == '\0')) {
      auxv = 5;
    }
    else {
      uv = uv + 0x40;
    }
LAB_80179238:
    deferred_cmd_post(0x400,uv,auxv);
  }
  else if (((*(uint *)(ctrl + 0x68) & 0x8000000) != 0) &&
          ((*(uint *)(ctrl + 0x5c) & 0x4000000) != 0)) {
    uv = *(uint *)(ctrl + 0x68) & 0x400;
    if ((ctrl[5] & 1U) == 0) {
      if (uv == 0) {
        auxv = 0x54;
        fidx = ctrl[3];
        tab = &DAT_801b4e44;
      }
      else {
        auxv = 0x55;
        fidx = ctrl[3];
        tab = &DAT_801b4e38;
      }
      fidx = tab[fidx];
      deferred_cmd_post(0x400,auxv,0);
    }
    else {
      if (uv == 0) {
        auxv = 0x54;
      }
      else {
        auxv = 0x55;
      }
      deferred_cmd_post(0x400,auxv,0);
      fidx = (&DAT_801b4e2c)[(byte)ctrl[3]];
    }
    uv = (uint)fidx;
    if ((0xbf < uv) && (ctrl[2] != '\0')) {
      uv = uv + 0x40;
    }
    auxv = 0;
    goto LAB_80179238;
  }
  uv = action_resolve(ctrl,motion);
  if (-1 < (int)uv) {
    dv = (short)(uv & 0xfff);
    *(short *)(motion_p + 0x80) = dv;
    if (0x8f < (uv & 0xfff)) {
      *(uint *)(ctrl + 0x5c) = *(uint *)(ctrl + 0x5c) & 0xefffbffc;
      *(short *)(motion_p + 0x14) = dv + -0x90;
    }
    if ((uv & 0x1000) == 0) {
      uv = *(uint *)(ctrl + 0x5c) & 0xffffffbf;
    }
    else {
      uv = *(uint *)(ctrl + 0x5c) | 0x40;
    }
    *(uint *)(ctrl + 0x5c) = uv;
  }
  action_commit(ctrl,motion_p);
  if ((((*(uint *)(ctrl + 0x6c) & 0x800) != 0) && ((ctrl[5] & 0x10U) == 0)) &&
     ((*(uint *)(ctrl + 0x6c) & 0x8000) == 0)) {
    *(ushort *)(ctrl + 0xd8) =
         *(short *)(ctrl + 0xd8) + (ushort)((byte)ctrl[0xda] >> 3) * 0x3c;
  }
  dir_rotate_set(((int)*(short *)(ctrl + 0x4e) - (int)*(short *)(motion_p + 0xd2)) +
               (int)*(short *)(ctrl + 0x8a),(int)*(short *)(ctrl + 0x8c),ctrl + 0xa2,
               ctrl + 0x9a);
  if (*(short *)(ctrl + 0x8c) < 1) {
    if ((-1 < *(short *)(ctrl + 0x8c)) ||
       (acc = *(short *)(ctrl + 0x8c) + *(short *)(ctrl + 0x8e),
       *(ushort *)(ctrl + 0x8c) = acc, (int)((uint)acc << 0x10) < 1)) goto LAB_8017938c;
    ctrl[0x8e] = '\0';
    ctrl[0x8f] = '\0';
  }
  else {
    acc = *(short *)(ctrl + 0x8c) + *(short *)(ctrl + 0x8e);
    *(ushort *)(ctrl + 0x8c) = acc;
    if (-1 < (int)((uint)acc << 0x10)) goto LAB_8017938c;
    ctrl[0x8e] = '\0';
    ctrl[0x8f] = '\0';
  }
  ctrl[0x8c] = '\0';
  ctrl[0x8d] = '\0';
LAB_8017938c:
  if (*(short *)(ctrl + 0xb2) != 0) {
    dir_rotate_acc((int)*(short *)(ctrl + 0xc4),(int)*(short *)(ctrl + 0xb2),ctrl + 0xa2,
                 ctrl + 0x9a);
    iv = (int)*(short *)(ctrl + 0xb2) * (int)*(short *)(ctrl + 0xb4);
    if (iv < 0) {
      iv = iv + 0x7f;
    }
    *(short *)(ctrl + 0xb2) = (short)(iv >> 7);
  }
  launch_vel_update(ctrl,motion_p);
  iv = (int)*(short *)(ctrl + 0x9a) * (int)*(short *)(ctrl + 0x88);
  if (iv < 0) {
    iv = iv + 0xf;
  }
  iv2 = (int)*(short *)(ctrl + 0x9c) * (int)*(short *)(ctrl + 0x88);
  *(short *)(ctrl + 0x9a) = (short)(iv >> 4);
  if (iv2 < 0) {
    iv2 = iv2 + 0xf;
  }
  iv = (int)*(short *)(ctrl + 0x9e) * (int)*(short *)(ctrl + 0x88);
  *(short *)(ctrl + 0x9c) = (short)(iv2 >> 4);
  if (iv < 0) {
    iv = iv + 0xf;
  }
  *(short *)(ctrl + 0x9e) = (short)(iv >> 4);
  return;

}

/* ---- action-commit + state helpers (action_commit family) ---- */
extern void motion_blend_frame(byte*,byte*);
extern int  atan2_lut(int,int);
extern void gte_op_a00428(int*,int*);
extern uint FUN_80194500(int);
undefined2 *euler_rotmat(uint,uint,uint,undefined2*);
void hitrec_table_clear(int);
void table_1f0ae0_clear(int);
extern void fighter_reset_fx(int,byte*);
extern void keyframe_blend_apply(byte*,byte*);
void keyframe_advance(byte*,byte*);
void skeleton_pose_step(byte*,undefined2*,byte*,int);
extern void fx_record_alloc(int,int,int,int,void*);
extern void quad_fx_spawn(int,int,undefined4*,undefined2,ushort,int,int);
extern void triple_track_spawn(int,int,int);
undefined8 vec2_rot_lut(undefined4,int);
void anim_lerp_scalar(short*,short,int,int);
void anim_lerp_vec3(short*,short*,int,int,short);
void anim_lerp_vec3_raw(short*,short*,int,int);
void anim_lerp_packed4(short*,ushort*,int,int);
extern void anim_lerp_aux(byte*,int,int,int);
void anim_lerp_packed3(short*,ushort*,int,int);
uint *rotmat_rtps_build(uint,uint*);


/* action_commit - commit resolved action: sync motion state,
 * advance timers/blends, steer toward foe, post deferred cmds. */
void action_commit(byte *ctrl, byte *motion)
{
  bool sign;
  ushort rem;
  uint f6c;
  int v;
  int raw;
  undefined4 res;
  int lead;
  int dv;
  int bias;
  undefined1 rotm [32];
  int dlt [4];
  
  if (*(short *)(motion + 0x80) != *(short *)(motion + 0x82)) {
    *(undefined2 *)(motion + 0x82) = *(undefined2 *)(motion + 0x80);
    action_switch_guest((int)guest_of(ctrl),(int *)motion,(int)*(short *)(motion + 0x80));
  }
  ev_stream_dispatch(ctrl,motion);
  *(undefined2 *)(ctrl + 0x88) = *(undefined2 *)(ctrl + 0x86);
  if ((((ctrl[5] & 1U) != 0) && ((*(uint *)(ctrl + 0x58) & 0xc0000000) == 0x80000000)) &&
     (*(short *)(motion + 0x80) != 0xbe)) {
    *(undefined2 *)(ctrl + 0x88) = *(undefined2 *)(ctrl + 0xfc);
  }
  motion_blend_frame(ctrl,motion);
  *(short *)(ctrl + 0xb6) = *(short *)(ctrl + 0xb6) + *(short *)(ctrl + 0x88);
  *(uint *)(ctrl + 0x6c) = *(uint *)(ctrl + 0x6c) & 0xfffff7ff;
  rem = *(ushort *)(ctrl + 0xba);
  if ((0 < *(short *)(ctrl + 0xba)) &&
     (*(short *)(ctrl + 0xba) = (short)((uint)rem - (uint)*(ushort *)(ctrl + 0x88)),
     (int)(((uint)rem - (uint)*(ushort *)(ctrl + 0x88)) * 0x10000) < 1)) {
    ctrl[0xba] = '\0';
    ctrl[0xbb] = '\0';
    ctrl[0x26] = '\0';
    *(uint *)(ctrl + 0x6c) = *(uint *)(ctrl + 0x6c) | 0x800;
  }
  rem = *(ushort *)(ctrl + 0xb8);
  if ((0 < *(short *)(ctrl + 0xb8)) &&
     (*(short *)(ctrl + 0xb8) = (short)((uint)rem - (uint)*(ushort *)(ctrl + 0x86)),
     (int)(((uint)rem - (uint)*(ushort *)(ctrl + 0x86)) * 0x10000) < 1)) {
    ctrl[0xb8] = '\0';
    ctrl[0xb9] = '\0';
    ctrl[0x26] = -1;
  }
  if ((*(int *)(ctrl + 0xb8) == 0) && (-1 < *(short *)(ctrl + 0xbc))) {
    *(short *)(ctrl + 0xbc) = *(short *)(ctrl + 0xbc) + *(short *)(ctrl + 0x88);
  }
  if (0 < *(short *)(ctrl + 0xd0)) {
    rem = *(ushort *)(ctrl + 0xd0);
    ctrl[0x27] = -1;
    *(short *)(ctrl + 0xd0) = (short)((uint)rem - (uint)*(ushort *)(ctrl + 0x88));
    if (((int)(((uint)rem - (uint)*(ushort *)(ctrl + 0x88)) * 0x10000) < 1) ||
       (*(short *)(motion + 0x80) != 1)) {
      ctrl[0xd0] = '\0';
      ctrl[0xd1] = '\0';
      ctrl[0x27] = '\0';
    }
  }
  if (((*(int *)(ctrl + 100) < 0) && (*(short *)(ctrl + 0xb8) < 0xc0)) &&
     (*(short *)(ctrl + 0xbc) <= *(short *)(ctrl + 0x88))) {
    f6c = *(uint *)(ctrl + 0x6c) | 0x4000;
  }
  else {
    f6c = *(uint *)(ctrl + 0x6c) & 0xffffbfff;
  }
  *(uint *)(ctrl + 0x6c) = f6c;
  if (0 < *(short *)(ctrl + 0xd2)) {
    if ((*(ushort *)(ctrl + 0x7a) & 0xf0e8) != 0) {
      *(short *)(ctrl + 0xd2) = *(short *)(ctrl + 0xd2) + *(short *)(ctrl + 0x88) * -2;
    }
    rem = *(ushort *)(ctrl + 0xd2);
    *(short *)(ctrl + 0xd2) = (short)((uint)rem - (uint)*(ushort *)(ctrl + 0x88));
    if ((int)(((uint)rem - (uint)*(ushort *)(ctrl + 0x88)) * 0x10000) < 1) {
      ctrl[0xd2] = '\0';
      ctrl[0xd3] = '\0';
    }
    pending_hit_a_clear(ctrl);
    pending_hit_b_clear(ctrl);
  }
  if ((0 < *(short *)(motion + 0x116)) &&
     (v = (uint)*(ushort *)(motion + 0x116) - (uint)*(ushort *)(ctrl + 0x88),
     *(short *)(motion + 0x116) = (short)v, v * 0x10000 < 1)) {
    v = (int)*(short *)(motion + 0x114);
    *(undefined2 *)(motion + 0x116) = 0;
    if ((-1 < v) && (*(int *)(ctrl + 0x58) < 0)) {
      if ((v < 0xc0) || (v = v + 0x40, ctrl[2] == '\0')) {
        v = (int)*(short *)(motion + 0x114);
      }
      deferred_cmd_post(0x400,v,0);
    }
  }
  if ((0 < *(short *)(motion + 0x110)) &&
     (v = (uint)*(ushort *)(motion + 0x110) - (uint)*(ushort *)(ctrl + 0x88),
     *(short *)(motion + 0x110) = (short)v, v * 0x10000 < 1)) {
    v = (int)*(short *)(motion + 0x10e);
    *(undefined2 *)(motion + 0x110) = 0;
    if (-1 < v) {
      if ((v < 0xc0) || (v = v + 0x40, ctrl[2] == '\0')) {
        v = (int)*(short *)(motion + 0x10e);
      }
      deferred_cmd_post(0x400,v,0);
    }
  }
  if (((*(uint *)(ctrl + 0x5c) & 0x10000) == 0) || (*ctrl < '\0')) {
    *(undefined2 *)(motion + 0x106) = 0;
    *(undefined2 *)(motion + 0x108) = 0;
  }
  else {
    v = atan2_lut(*(int *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0x3c) -
                         *(int *)((char *)guest_at(*(uint32_t *)((char *)guest_at(*(uint32_t *)(ctrl + 0x2c)) + 0x20)) + 0x3c),
                         *(int *)((char *)guest_at(*(uint32_t *)((char *)guest_at(*(uint32_t *)(ctrl + 0x2c)) + 0x20)) + 0x34) -
                         *(int *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0x34));
    lead = (int)((uint)*(ushort *)((char *)guest_at(*(uint32_t *)(ctrl + 0x40)) + 0x3e) << 0x10) >> 0x11;
    dv = ((v - *(short *)(ctrl + 0x4e)) + lead) * 0x100000 >> 0x14;
    v = dv;
    if (dv < 0) {
      v = -dv;
    }
    if ((0x31c < v) && (sign = -1 < dv, dv = -0x31c, sign)) {
      dv = 0x31c;
    }
    v = dv;
    if (dv < 0) {
      v = -dv;
    }
    bias = 0;
    if (0x400 < v) {
      raw = -v + 0x800;
      bias = raw >> 3;
      if (raw < 0) {
        bias = (-v + 0x807) >> 3;
      }
    }
    dv = dv - ((lead + *(short *)(motion + 0x106)) * 0x100000 >> 0x14);
    v = dv;
    if (dv < 0) {
      v = -dv;
    }
    if ((0x2d < v) && (sign = -1 < dv, dv = -0x2d, sign)) {
      dv = 0x2d;
    }
    *(short *)(motion + 0x106) = *(short *)(motion + 0x106) + (short)dv;
    dlt[1] = 0;
    dlt[0] = *(int *)((char *)guest_at(*(uint32_t *)((char *)guest_at(*(uint32_t *)(ctrl + 0x2c)) + 0x20)) + 0xd4) -
                  *(int *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0xd4);
    dlt[2] = *(int *)((char *)guest_at(*(uint32_t *)((char *)guest_at(*(uint32_t *)(ctrl + 0x2c)) + 0x20)) + 0xdc) -
                  *(int *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0xdc);
    gte_op_a00428(dlt,dlt);
    dv = *(int *)((char *)guest_at(*(uint32_t *)((char *)guest_at(*(uint32_t *)(ctrl + 0x2c)) + 0x20)) + 0xd8);
    v = *(int *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0xd8);
    res = FUN_80194500(dlt[0] + dlt[2]);
    v = atan2_lut(res,dv - v);
    dv = atan2_lut((int)*(short *)((char *)guest_at(*(uint32_t *)(ctrl + 0x40)) + 0x3a),
                         (int)*(short *)((char *)guest_at(*(uint32_t *)(ctrl + 0x40)) + 0x3c));
    v = (v + dv + bias) * 0x100000 >> 0x14;
    if (v < -0x271) {
      v = -0x271;
    }
    else if (0x1c7 < v) {
      v = 0x1c7;
    }
    v = v - ((dv + *(short *)(motion + 0x108)) * 0x100000 >> 0x14);
    dv = v;
    if (v < 0) {
      dv = -v;
    }
    if ((0x22 < dv) && (sign = -1 < v, v = -0x22, sign)) {
      v = 0x22;
    }
    v = (uint)*(ushort *)(motion + 0x108) + v;
    *(short *)(motion + 0x108) = (short)v;
    euler_rotmat(v * 0x10000 >> 0x10,-(int)*(short *)(motion + 0x106),0,rotm);
    matrix_vec3_apply(rotm,(uint *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0xa4));
  }
  return;

}

/* pending_hit_a_clear - clear pending-hit-A record on the
 * ctrl+0x38 motion object and the slot hitrec table. */
void pending_hit_a_clear(byte *ctrl)
{
  uintptr_t mo;
  
  mo = (uintptr_t)guest_at(*(uint32_t *)(ctrl + 0x38));
  *(undefined4 *)(mo + 0x18) = 0;
  *(undefined2 *)(mo + 0x1c) = 0;
  *(undefined2 *)(mo + 0x1e) = 0;
  hitrec_table_clear(*(undefined1 *)(ctrl + 2));
  return;

}

/* hit_chain_clear - clear hit-chain counters on the ctrl+0x38
 * motion object and the chain table. */
void hit_chain_clear(byte *ctrl)
{
  uintptr_t mo;
  
  mo = (uintptr_t)guest_at(*(uint32_t *)(ctrl + 0x38));
  *(undefined1 *)(mo + 0x7d) = 0;
  *(undefined1 *)(mo + 0x7c) = 0;
  *(undefined1 *)(mo + 0x7e) = 0xff;
  table_1f0ae0_clear(*(undefined1 *)(ctrl + 2));
  return;

}

/* pending_hit_b_clear - clear pending-hit-B record on the
 * ctrl+0x38 motion object. */
void pending_hit_b_clear(byte *ctrl)
{
  uintptr_t mo;
  
  mo = (uintptr_t)guest_at(*(uint32_t *)(ctrl + 0x38));
  *(undefined4 *)(mo + 0x20) = 0;
  *(undefined2 *)(mo + 0x24) = 0xffff;
  *(undefined2 *)(mo + 0x26) = 0;
  *(undefined2 *)(mo + 0x2a) = 0xffff;
  return;

}

/* fighter_combat_reset - full combat-state reset: deferred cmd,
 * timers, flags, slots, reset fx, hit records. */
void fighter_combat_reset(byte *ctrl)
{
  deferred_cmd_post(0x400,7,0);
  *(undefined2 *)(ctrl + 0xd6) = 0;
  *(undefined2 *)(ctrl + 0xd8) = 0;
  *(undefined1 *)(ctrl + 5) = 0;
  *(uint *)(ctrl + 0x6c) = *(uint *)(ctrl + 0x6c) & 0xffff7fff;
  *(undefined2 *)(ctrl + 0xae) = *(undefined2 *)(ctrl + 0xac);
  slot_apply_guest((int)guest_of(ctrl),0);
  fighter_reset_fx(1,ctrl);
  hit_chain_clear(ctrl);
  pending_hit_a_clear(ctrl);
  pending_hit_b_clear(ctrl);
  return;

}

/* trig LUT readers (guest sin/cos) — host binds br2_lut tables */

void trig_lut_read(uint32_t ctrl_g, byte *motion)
{
  int angle;
  int vel;
  
  angle = (((int)((uint)*(byte *)guest_at(ctrl_g + 0xdb) * 0x60) >> 3) + 0x200) *
          (0x41 - (uint)*(byte *)guest_at(ctrl_g + 0xdd));
  if (angle < 0) {
    angle = angle + 0x3f;
  }
  *(undefined2 *)(motion + 0xb4) = 0x78;
  *(undefined2 *)(motion + 0xb2) = 0;
  *(undefined2 *)(motion + 0x92) = 0xa0;
  vel = lut_cos_q12(angle >> 6);
  vel = vel * 0xd00;
  if (vel < 0) {
    vel = vel + 0xfff;
  }
  *(short *)(motion + 0x90) = (short)(vel >> 0xc);
  angle = lut_sin_q12(angle >> 6);
  angle = angle * -0x500;
  if (angle < 0) {
    angle = angle + 0xfff;
  }
  *(short *)(motion + 0x98) = (short)(angle >> 0xc);
  if (((*(byte *)guest_at(ctrl_g + 5) & 0x10) != 0) && ((*(uint *)guest_at(ctrl_g + 100) & 0x480000) != 0)) {
    angle = *(short *)(motion + 0x90) * 0x140;
    if (angle < 0) {
      angle = angle + 0xff;
    }
    *(short *)(motion + 0x90) = (short)((uint)angle >> 8);
  }
  *(short *)(motion + 0x90) =
       (short)mips_div_lo(*(short *)(motion + 0x90) * 0x12c0, *(short *)(motion + 0x24) + 4000);
  *(short *)(motion + 0x98) =
       (short)mips_div_lo(*(short *)(motion + 0x98) * 0x12c0, *(short *)(motion + 0x24) + 4000);
  return;

}

/* trig_vel_flat - set launch velocity (motion+0x90/0x98) for a
 * flat attack from facing byte; scaled by height (+0x24). */
void trig_vel_flat(uint32_t ctrl_g, byte *motion)
{
  short vel;
  int angle;
  
  angle = ((uint)*(byte *)guest_at(ctrl_g + 0xdb) * 8 + 0x400) * (0x41 - (uint)*(byte *)guest_at(ctrl_g + 0xdd));
  if (angle < 0) {
    angle = angle + 0x3f;
  }
  *(undefined2 *)(motion + 0xb4) = 0x78;
  *(undefined2 *)(motion + 0xb2) = 0;
  *(undefined2 *)(motion + 0x92) = 0xa0;
  angle = lut_cos_q12(angle >> 6);
  vel = (short)(angle / 2);
  *(short *)(motion + 0x90) = vel;
  *(undefined2 *)(motion + 0x98) = 0;
  *(short *)(motion + 0x90) = (short)mips_div_lo(vel * 0x12c0, *(short *)(motion + 0x24) + 4000);
  return;

}

/* trig_vel_up - launch velocity for rising attack; extra -0x2000
 * z-kick when the +5&0x10 flag is set. */
void trig_vel_up(uint32_t ctrl_g, byte *motion)
{
  undefined2 cos_v;
  short sin_v;
  int angle;
  
  angle = ((int)((uint)*(byte *)guest_at(ctrl_g + 0xdb) * 0x10) >> 2) + 0x300;
  *(undefined2 *)(motion + 0xb4) = 0x78;
  *(undefined2 *)(motion + 0xb2) = 0;
  *(undefined2 *)(motion + 0x92) = 0xa0;
  cos_v = lut_cos_q12(angle);
  *(undefined2 *)(motion + 0x90) = cos_v;
  sin_v = lut_sin_q12(angle);
  *(short *)(motion + 0x98) = sin_v * -2;
  if ((*(byte *)guest_at(ctrl_g + 5) & 0x10) != 0) {
    *(short *)(motion + 0x98) = sin_v * -2 + -0x2000;
  }
  *(short *)(motion + 0x90) =
       (short)mips_div_lo(*(short *)(motion + 0x90) * 0x12c0, *(short *)(motion + 0x24) + 4000);
  *(short *)(motion + 0x98) =
       (short)mips_div_lo(*(short *)(motion + 0x98) * 0x12c0, *(short *)(motion + 0x24) + 4000);
  return;

}

/* trig_vel_diag - launch velocity for diagonal attack (cos on x,
 * -sin on z). */
void trig_vel_diag(uint32_t ctrl_g, byte *motion)
{
  undefined2 cos_v;
  short sin_v;
  int angle;
  
  angle = (int)((uint)*(byte *)guest_at(ctrl_g + 0xdb) * 0x10) >> 2;
  *(undefined2 *)(motion + 0xb4) = 0x78;
  *(undefined2 *)(motion + 0xb2) = 0;
  *(undefined2 *)(motion + 0x92) = 0xa0;
  cos_v = lut_cos_q12(angle);
  *(undefined2 *)(motion + 0x90) = cos_v;
  sin_v = lut_sin_q12(angle);
  *(short *)(motion + 0x98) = -sin_v;
  *(short *)(motion + 0x90) =
       (short)mips_div_lo(*(short *)(motion + 0x90) * 0x12c0, *(short *)(motion + 0x24) + 4000);
  *(short *)(motion + 0x98) =
       (short)mips_div_lo(*(short *)(motion + 0x98) * 0x12c0, *(short *)(motion + 0x24) + 4000);
  return;

}
/* launch_vel_update - integrate launch velocity with turn assist;
 * writes ctrl+0x90 velocity and ctrl+0x9c remainder. */
void launch_vel_update(byte *ctrl,byte *motion)
{
  ushort acc;
  int state;
  int vx;
  int vy;
  int rem;
  vx = (int)*(short *)(ctrl + 0x90);
  vy = (int)*(short *)(ctrl + 0x92);
  state = *(int *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0x38);
  rem = 0;
  if ((*(int *)(ctrl + 0x5c) < 0) && (state < *(short *)(ctrl + 0x94))) {
    vy = 0xa0;
    *(undefined2 *)(ctrl + 0x92) = 0xa0;
  }
  if ((state == *(short *)(ctrl + 0x94)) && (vx == 0 && vy == 0)) {
    *(undefined2 *)(ctrl + 0x9c) = 0;
    *(undefined2 *)(ctrl + 0xa4) = 0;
    return;
  }
  if (((*(uint *)(ctrl + 0x60) & 0x2000) == 0) && (vx != 0 || vy != 0)) {
    dir_rotate_acc((int)*(short *)(motion + 0xd0) + (int)*(short *)(motion + 0xe4) +
                 (int)*(short *)(ctrl + 0x96),(int)*(short *)(ctrl + 0x98),ctrl + 0xa2,
                 ctrl + 0x9a);
    if (*(short *)(ctrl + 0x98) < 1) {
      if ((*(short *)(ctrl + 0x98) < 0) &&
         (acc = *(short *)(ctrl + 0x98) + *(short *)(ctrl + 0x8e),
         *(ushort *)(ctrl + 0x98) = acc, 0 < (int)((uint)acc << 0x10))) {
        *(undefined2 *)(ctrl + 0x8e) = 0;
        goto LAB_8017957c;
      }
    }
    else {
      acc = *(short *)(ctrl + 0x98) + *(short *)(ctrl + 0x8e);
      *(ushort *)(ctrl + 0x98) = acc;
      if ((int)((uint)acc << 0x10) < 0) {
        *(undefined2 *)(ctrl + 0x8e) = 0;
LAB_8017957c:
        *(undefined2 *)(ctrl + 0x98) = 0;
      }
    }
  }
  if (((*(uint *)(ctrl + 0x60) & 0x800) == 0) && (vx < 1)) {
    vy = vy * *(short *)(ctrl + 0x88);
    if (vy < 0) {
      vy = vy + 0xf;
    }
    vy = vx + (vy >> 4);
  }
  else {
    if (((*(uint *)(ctrl + 0x60) & 0x1000) != 0) || (vx < 1)) goto LAB_80179618;
    vy = vy * *(short *)(ctrl + 0x88);
    if (vy < 0) {
      vy = vy + 0xf;
    }
    vy = vx + (vy >> 4);
    if (vy < -0x8000) {
      vy = -0x8000;
    }
  }
  *(short *)(ctrl + 0x90) = (short)vy;
  rem = (short)(vx >> 7);
LAB_80179618:
  *(undefined2 *)(ctrl + 0x9c) = rem;
  return;

}

/* motion_turn_update - turn-rate -> flag bands + per-frame turn
 * step into ctrl+0x74/0x4e and motion+0xe4. */
void motion_turn_update(byte *ctrl,byte *motion)
{
  ushort ctr16;
  uint flags;
  int abs_rate;
  int rate;
  int step;
  rate = (int)*(short *)(ctrl + 0x74);
  abs_rate = rate;
  if (rate < 0) {
    abs_rate = -rate;
  }
  step = 0;
  if (abs_rate < 0x401) {
    flags = *(uint *)(ctrl + 0x5c) & 0xffbfffff;
  }
  else {
    flags = *(uint *)(ctrl + 0x5c) | 0x400000;
  }
  *(uint *)(ctrl + 0x5c) = flags;
  rate = (rate + *(short *)(motion + 0xd2)) * 0x100000 >> 0x14;
  abs_rate = rate;
  if (rate < 0) {
    abs_rate = -rate;
  }
  if (abs_rate < 0x2ab) {
    flags = *(uint *)(ctrl + 0x5c) & 0xfffffbff;
  }
  else {
    flags = *(uint *)(ctrl + 0x5c) | 0x400;
  }
  *(uint *)(ctrl + 0x5c) = flags;
  abs_rate = rate;
  if (rate < 0) {
    abs_rate = -rate;
  }
  if (abs_rate < 0x556) {
    flags = *(uint *)(ctrl + 0x5c) & 0xffefffff;
  }
  else {
    flags = *(uint *)(ctrl + 0x5c) | 0x100000;
  }
  *(uint *)(ctrl + 0x5c) = flags;
  if ((*(uint *)((char *)guest_at(*(uint32_t *)(ctrl + 0x2c)) + 0x60) & 0x4000000) == 0) {
    if ((*(uint *)(ctrl + 0x5c) & 0x20000) == 0) {
      if ((*(uint *)(ctrl + 0x60) & 0x2000000) != 0) {
        abs_rate = rate;
        if (rate < 0) {
          abs_rate = -rate;
        }
        if (0x155 < abs_rate) {
          if (abs_rate < 0x6aa) goto LAB_801798e0;
          rate = (rate + -0x800) * 0x100000 >> 0x14;
        }
        abs_rate = rate;
        if (rate < 0) {
          abs_rate = -rate;
        }
        step = rate;
        if ((0x22 < abs_rate) && (step = 0x22, rate < 0)) {
          step = -0x22;
        }
      }
    }
    else {
      if ((*(uint *)(ctrl + 0x5c) & 0x8000000) == 0) {
        abs_rate = rate;
        if (rate < 0) {
          abs_rate = -rate;
        }
        if ((*(short *)(motion + 0x10c) <= abs_rate) && ((*(ushort *)(motion + 0x10a) & 0x80) == 0))
        goto LAB_801798e0;
      }
      else {
        abs_rate = rate;
        if (rate < 0) {
          abs_rate = -rate;
        }
        if ((abs_rate <= *(short *)(motion + 0x10c)) && ((*(ushort *)(motion + 0x10a) & 0x80) == 0))
        goto LAB_801798e0;
        rate = (rate + -0x800) * 0x100000 >> 0x14;
      }
      abs_rate = rate;
      if (rate < 0) {
        abs_rate = -rate;
      }
      step = rate;
      if ((0xb6 < abs_rate) && (step = 0xb6, rate < 0)) {
        step = -0xb6;
      }
    }
  }
LAB_801798e0:
  ctr16 = *(ushort *)(motion + 0x10a) - 1;
  if (((*(ushort *)(motion + 0x10a) & 0x7f) != 0) &&
     (*(ushort *)(motion + 0x10a) = ctr16, (ctr16 & 0x7f) == 0)) {
    *(uint *)(ctrl + 0x5c) = *(uint *)(ctrl + 0x5c) & 0xfffdffff;
  }
  if (step != 0) {
    *(short *)(ctrl + 0x74) = (short)((*(short *)(ctrl + 0x74) - step) * 0x100000 >> 0x14);
    *(short *)(ctrl + 0x4e) = (short)((*(short *)(ctrl + 0x4e) + step) * 0x100000 >> 0x14);
    *(short *)(motion + 0xe4) = (short)((*(short *)(motion + 0xe4) + step) * 0x100000 >> 0x14);
  }
  return;

}

/* motion_blend_frame - advance keyframe anim, copy joint pose,
 * apply yaw rotation to motion position. */
void motion_blend_frame(byte *ctrl,byte *motion)
{
  short yaw;
  undefined2 w;
  short pz;
  int sn;
  int cs;
  undefined2 *jdst;
  undefined2 *jsrc;
  int dy;
  int t;
  int v;
  int g;
  if (*(int *)(motion + 0xb0) == 0) goto LAB_8018604c;
  if (*(short *)(motion + 0xce) == 0) {
LAB_80186010:
    keyframe_blend_apply(ctrl,motion);
  }
  else {
    if (*(short *)(motion + 0xca) < 1) {
      *(short *)(motion + 0xc6) = *(short *)(motion + 0xc6) + 1;
      keyframe_advance(ctrl,motion);
    }
    if (*(short *)(motion + 0xce) == 0) {
      if (*(short *)(motion + 0xc0) == 0) goto LAB_80186010;
    }
    else {
      keyframe_blend_apply(ctrl,motion);
      *(short *)(motion + 0xca) = *(short *)(motion + 0xca) - *(short *)(motion + 0xce);
    }
  }
  if (*(short *)(motion + 0xc4) < *(short *)(motion + 0xc2)) {
    *(short *)(motion + 0xc4) = *(short *)(motion + 0xc4) + *(short *)(ctrl + 0x88);
  }
LAB_8018604c:
  t = 0;
  jdst = (undefined2 *)((char *)guest_at(*(uint32_t *)(ctrl + 0x40)) + 0x12);
  jsrc = (undefined2 *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0x3c);
  do {
    jdst[-2] = jsrc[-4];
    t = t + 1;
    jdst[-1] = jsrc[-2];
    w = *jsrc;
    jsrc = jsrc + 0x28;
    *jdst = w;
    jdst = jdst + 0xe;
  } while (t < 0x11);
  skeleton_pose_step((byte *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0x50,
               (undefined2 *)((char *)guest_at(*(uint32_t *)(ctrl + 0x40)) + 0x1c),
               (byte *)((char *)guest_at(*(uint32_t *)(ctrl + 0x48)) + 0x20),0x12);
  g = *(int *)(ctrl + 0x20);
  t = -(int)*(short *)(motion + 0xe4);
  if (t != 0) {
    v = (int)*(short *)(motion + 0xd4) - *(int *)((char *)guest_at((uint32_t)g) + 0x34);
    dy = (int)*(short *)(motion + 0xd8) - *(int *)((char *)guest_at((uint32_t)g) + 0x3c);
    sn = lut_sin_q12(t);
    cs = lut_cos_q12(t);
    *(short *)(motion + 0xd4) =
         *(short *)((char *)guest_at((uint32_t)g) + 0x34) + (short)((dy * sn + v * cs) >> 0xc);
    sn = lut_cos_q12(t);
    t = lut_sin_q12(t);
    pz = *(short *)((char *)guest_at((uint32_t)g) + 0x3c);
    yaw = *(short *)(motion + 0xe4);
    *(undefined2 *)(motion + 0xe4) = 0;
    *(short *)(motion + 0xd0) = (short)((*(short *)(motion + 0xd0) + yaw) * 0x10) >> 4;
    *(short *)(motion + 0xd8) = pz + (short)((dy * sn - v * t) >> 0xc);
  }
  t = ((int)*(short *)(motion + 0xd0) + (int)*(short *)(motion + 0xd2) +
          (int)*(short *)(motion + 0xe4)) * 0x100000;
  *(short *)(ctrl + 0x4e) = (short)(t >> 0x14);
  euler_rotmat((int)*(short *)(ctrl + 0x4c),-(t >> 0x14),(int)*(short *)(ctrl + 0x50),
               (undefined2 *)((char *)guest_at((uint32_t)g) + 4));
  t = *(int *)(ctrl + 0x20);
  if ((*(uint *)(ctrl + 0x54) & 8) == 0) {
    g = *(int *)(ctrl + 0x2c);
    if ((*(uint *)((char *)guest_at((uint32_t)g) + 0x54) & 8) == 0) {
      if ((*(uint *)(ctrl + 0x54) & 0x10) == 0) {
        if ((*(uint *)((char *)guest_at((uint32_t)g) + 0x54) & 0x10) == 0) {
          return;
        }
        if (((*(uint *)(ctrl + 0x58) & 0x40100000) == 0) &&
           ((*(uint *)((char *)guest_at((uint32_t)g) + 0x58) & 0x40100000) == 0)) {
          return;
        }
      }
      g = *(short *)((char *)guest_at((uint32_t)t) + 0x6a) * 7;
      if (g < 0) {
        g = g + 7;
      }
      *(short *)((char *)guest_at((uint32_t)t) + 0x6a) = (short)(g >> 3);
      return;
    }
    if (((*(uint *)(ctrl + 0x58) & 0x40100000) == 0) &&
       ((*(uint *)((char *)guest_at((uint32_t)g) + 0x58) & 0x40100000) == 0)) {
      return;
    }
  }
  g = (uint)*(ushort *)((char *)guest_at((uint32_t)t) + 0x6a) << 0x10;
  *(short *)((char *)guest_at((uint32_t)t) + 0x6a) = (short)(((g >> 0x10) - (g >> 0x1f)) >> 1);
  return;

}

/* deferred_cmd_post - post or (0x1000) remove a deferred command
 * in the 0x20-slot ring at DAT_801f3600. */
void deferred_cmd_post(uint cmd,int arg,short aux)
{
  int i;
  ushort *slotw;
  short *slot;
  if (((cmd & 0xff) != 0) || (-1 < arg)) {
    if (cmd == 0) {
      deferred_cmd_reset();
    }
    else if ((cmd & 0x1000) == 0) {
      i = 0;
      if (_DAT_801f35f4 < 0x20) {
        slot = (short *)&DAT_801f3600;
        do {
          i = i + 1;
          if (*slot == 0) break;
          slot = slot + 3;
        } while (i < 0x20);
        *slot = (short)cmd;
        slot[1] = (short)arg;
        slot[2] = aux;
        _DAT_801f35f4 = _DAT_801f35f4 + 1;
      }
    }
    else {
      slotw = (ushort *)&DAT_801f3600;
      i = 0;
      do {
        if (((*slotw & 0xfff) == (cmd & 0xfff)) && ((short)slotw[1] == arg)) {
          *slotw = 0;
          _DAT_801f35f4 = _DAT_801f35f4 + -1;
        }
        i = i + 1;
        slotw = slotw + 3;
      } while (i < 0x20);
    }
  }
  return;

}

/* range_circle_test - point vs packed circle/rect box; returns
 * 0 on hit else box radius tag. */
int range_circle_test(int x,int y,uint *box,int radius)
{
  uint packed;
  packed = box[1];
  if (radius < 10) {
    if ((*box & 0xffff) == 0) {
      if (y < 0) {
        y = -y;
      }
      if (y <= (int)(packed >> 0x10)) {
        if (x < 0) {
          x = -x;
        }
        if (x <= (int)(packed & 0xffff)) {
          return 0;
        }
      }
    }
    else if (-1 < (int)((packed - x * x) - y * y)) {
      return 0;
    }
  }
  return (int)*box >> 0x10;

}

/* atan2_lut - fixed-point atan2 via octant + LUT at DAT_80123844. */
int atan2_lut(int x,int y)
{
  bool neg;
  int ang;
  int hi;
  int quad;
  quad = 0;
  ang = 0;
  if (x != 0 || y != 0) {
    ang = 0x800;
    neg = y < 1;
    if (neg) {
      y = -y;
      ang = -0x800;
    }
    if (x < 0) {
      x = -x;
      neg = !neg;
      quad = ang;
    }
    hi = x;
    if (x < y) {
      neg = (bool)(neg ^ 1);
      quad = ang >> 1;
      hi = y;
      y = x;
    }
    ang = (int)*(short *)(&DAT_80123844 + ((y << 0xc) / hi >> 2) * 2);
    if (neg) {
      ang = -ang;
    }
  }
  return quad + ang;

}


/* hitrec_table_clear - zero the 0x20 hit-record entries for a
 * fighter slot (DAT_801f0db0). */
void hitrec_table_clear(int slot)
{
  int i;
  i = 0;
  slot = slot << 8;
  do {
    i = i + 1;
    *(undefined2 *)(&DAT_801f0db0 + slot) = 0;
    *(undefined2 *)((char *)guest_at((uint32_t)slot + 0x801f0db2)) = 0;
    *(undefined2 *)(&DAT_801f0db4 + slot) = 0;
    *(undefined2 *)((char *)guest_at((uint32_t)slot + 0x801f0db6)) = 0;
    slot = slot + 8;
  } while (i < 0x20);
  return;

}


/* slot_mark2_set - set DAT_801f0ad8[slot] = 2. */
void slot_mark2_set(int slot)
{
  (&DAT_801f0ad8)[slot] = 2;
  return;

}

/* fighter_reset_fx - spawn reset FX (record alloc + quad fx +
 * triple track) when DAT_1f8000b1 clear and reset==1. */
void fighter_reset_fx(int reset,byte *ctrl)
{
  if ((DAT_1f8000b1 == '\0') && (reset == 1)) {
    fx_record_alloc(*(int *)(ctrl + 0x20) + 0x124,6,0x8000000e,0x83,&DAT_801b1d48);
    quad_fx_spawn((int)guest_of(ctrl),0x12,(undefined4 *)guest_at((uint32_t)(*(int *)(ctrl + 0x20) + 0x124)),0x420,0x3000,2,2);
    triple_track_spawn(*(undefined1 *)(ctrl + 2),0x30,0);
  }
  return;

}
/* lut_cos_q12 - Q12 cos over DAT_801247a8; 0x400 units = 90 deg. */
int lut_cos_q12(int angle)
{
  int idx;
  uint q;
  
  q = angle + 0x400;
  idx = (q & 0x3ff) * 2;
  if ((q & 0x400) != 0) {
    idx = (q & 0x3ff) * -2 + 0x800;
  }
  if ((q & 0x800) != 0) {
    return -(int)*(short *)(&DAT_801247a8 + idx);
  }
  return (int)*(short *)(&DAT_801247a8 + idx);

}

/* lut_sin_q12 - Q12 sin over DAT_801247a8. */
int lut_sin_q12(uint angle)
{
  int idx;
  
  idx = (angle & 0x3ff) * 2;
  if ((angle & 0x400) != 0) {
    idx = (angle & 0x3ff) * -2 + 0x800;
  }
  if ((angle & 0x800) != 0) {
    return -(int)*(short *)(&DAT_801247a8 + idx);
  }
  return (int)*(short *)(&DAT_801247a8 + idx);

}

/* dir_rotate_wrap - rotate (vec.x,vec.z) by angle scaled by mag
 * via vec2_rot_lut, wrap to +/-0x40; returns quotients. */
unsigned long long dir_rotate_wrap(undefined4 angle,int mag,short *vec)
{
  ushort wx;
  ushort wz;
  int nx;
  uint carry;
  int t;
  int oz;
  undefined8 rot;
  
  if (mag == 0) {
    return 0;
  }
  t = (int)*vec;
  oz = (int)vec[2];
  rot = vec2_rot_lut(angle,mag);
  nx = (int)((ulonglong)rot >> 0x20) + t + 0x40;
  t = (int)rot + oz + 0x40;
  wx = (ushort)nx;
  wz = (ushort)t;
  carry = nx >> 7;
  *vec = (wx - (wx & 0xff80)) + -0x40;
  vec[2] = (wz - (wz & 0xff80)) + -0x40;
  return CONCAT44(carry,(t >> 7) << 0x10 | carry & 0xffff) & 0xffffffffffff;

}

/* keyframe_blend_apply - blend keyframe pose channels into
 * motion/skeleton state at blend alpha. */
void keyframe_blend_apply(byte *ctrl,byte *motion)
{
  extern void decomp_trace_record(uint8_t,uint32_t,uint32_t,uint8_t,uint32_t);
  byte tag;
  short w;
  byte *rec;
  int adata;
  int skel;
  short *psrc;
  short *pdst;
  int *mp;
  int cur;
  int j;
  int i;
  int alpha;
  uint slot;
  int krec;
  
  slot = (uint)*(byte *)(motion + 0x100);
  krec = *(int *)(motion + slot * 4 + 0xf8);
  if ((int)*(short *)(motion + 0xce) < (int)*(short *)(motion + 0xca)) {
    /* guest div @0x801859F8: ca==0 -> LO = (dividend<0)?1:-1, no trap */
    alpha = mips_div_lo((int)*(short *)(motion + 0xce) << 0xc,
                         (int)*(short *)(motion + 0xca));
  }
  else {
    alpha = 0x1000;
  }
  anim_lerp_scalar((short *)(motion + 0xd2),(int)*(short *)guest_at(*(uint32_t *)(motion + 0xb8)),slot,alpha);
  anim_lerp_vec3((short *)(motion + 0xdc),(short *)((char *)guest_at(*(uint32_t *)(motion + 0xb8)) + 2),slot,alpha,0x80);
  anim_lerp_vec3((short *)((char *)guest_at(*(uint32_t *)(ctrl + 0x40)) + 0x24),(short *)((char *)guest_at(*(uint32_t *)(motion + 0xb8)) + 8),slot,alpha,
               (int)*(short *)(motion + 0xbe));
  anim_lerp_vec3_raw((short *)((char *)guest_at(*(uint32_t *)(ctrl + 0x40)) + 0x78),(short *)((char *)guest_at(*(uint32_t *)(motion + 0xb8)) + 0xe),slot,alpha);
  skel = *(int *)(ctrl + 0x40);
  adata = *(int *)(motion + 0xb8);
  anim_lerp_packed4((short *)guest_at((uint32_t)(skel + 0x1c)),(ushort *)guest_at((uint32_t)(adata + 0x14 + (uint)*(byte *)((char *)guest_at((uint32_t)krec) + 1) * 6)),0,alpha);
  skel = skel + 0x38;
  i = 2;
  do {
    j = i;
    cur = skel;
    i = j + 1;
    anim_lerp_packed4((short *)guest_at((uint32_t)cur),(ushort *)guest_at((uint32_t)(adata + 0x14 + (uint)*(byte *)((char *)guest_at((uint32_t)krec) + j) * 6)),slot,alpha);
    skel = cur + 0x1c;
  } while (i < 0x11);
  anim_lerp_aux((byte *)guest_at((uint32_t)(cur + 0x1c)),
               (int)*(short *)guest_at((uint32_t)((uint)*(byte *)((char *)guest_at((uint32_t)krec) + i) * 2 + adata + 0x74 + -0x20)),slot,
               alpha);
  anim_lerp_aux((byte *)guest_at((uint32_t)(cur + 0x38)),
               (int)*(short *)guest_at((uint32_t)((uint)*(byte *)((char *)guest_at((uint32_t)krec) + j + 2) * 2 + adata + 0x74 + -0x20)),
               slot,alpha);
  skel = *(int *)(ctrl + 0x40);
  tag = *(byte *)((char *)guest_at((uint32_t)adata) + 0x78);
  rec = (byte *)guest_at((uint32_t)adata) + 0x78;
  while (tag != 0xff) {
    {
      uint32_t _idx = (uint)*(byte *)((char *)guest_at((uint32_t)krec) + (uint)tag);
      uint32_t _dst = (uint32_t)skel + _idx * 0x1c + 0x24;
      if (_dst >= 0x801d0000u && _dst < 0x80200000u)
        decomp_trace_record(52, _dst, (uint32_t)skel, (uint8_t)_idx, 0);
    }
    anim_lerp_packed3((short *)guest_at((uint32_t)(skel + (uint)*(byte *)((char *)guest_at((uint32_t)krec) + (uint)tag) * 0x1c + 0x24)),(ushort *)rec,slot,alpha);
    tag = rec[4];
    rec = rec + 4;
  }
  krec = 1;
  pdst = (short *)((char *)guest_at(*(uint32_t *)(ctrl + 0x20)) + 0x6c);
  psrc = (short *)((char *)guest_at(*(uint32_t *)(ctrl + 0x40)) + 0x28);
  mp = (int *)guest_at(*(uint32_t *)((char *)guest_at(*(uint32_t *)(motion + 0x10)) + (*(byte *)(ctrl + 5) & 1) * 0x1c + 8) +
                  *(uint32_t *)(ctrl + 0x30));
  do {
    krec = krec + 1;
    alpha = *mp;
    mp = mp + 1;
    pdst[-2] = (short)((alpha << 0x16) >> 0x16) + (psrc[-2] >> 4);
    pdst[-1] = (short)((alpha << 0xc) >> 0x16) + (psrc[-1] >> 4);
    w = *psrc;
    psrc = psrc + 0xe;
    *pdst = (short)((alpha << 2) >> 0x16) + (w >> 4);
    pdst = pdst + 0x28;
  } while (krec < 0x13);
  return;

}

/* keyframe_advance - advance keyframe timeline; stage next
 * frame and open the blend window. */
void keyframe_advance(byte *ctrl,byte *motion)
{
  short step;
  undefined4 kf;
  undefined2 lo;
  undefined2 hi;
  
  kf = *(undefined4 *)((char *)guest_at(*(uint32_t *)(motion + 0xb4)) + *(short *)(motion + 0xc6) * 4);
  step = *(short *)(motion + 0xcc);
  *(short *)(motion + 200) = step;
  if (step == 0) {
    *(undefined2 *)(motion + 0xce) = 0;
    *(undefined2 *)(motion + 0xca) = 0;
  }
  else {
    *(short *)(motion + 0xca) = *(short *)(motion + 0xca) + step;
    *(undefined2 *)(motion + 0xce) = *(undefined2 *)(ctrl + 0x88);
    lo = (short)kf;
    *(int *)(motion + 0xb8) = (int)lo + *(int *)(motion + 0xb0);
    hi = (short)((uint)kf >> 0x10);
    *(short *)(motion + 0xcc) = hi << 4;
  }
  return;

}

/* skeleton_pose_step - per-joint skeleton pose step: copy pose
 * channels, compute part matrices, build root rotmat. */
void skeleton_pose_step(byte *joints,undefined2 *pose,byte *parts,int count)
{
  ushort w;
  ushort *pp;
  byte *jp;
  int i;
  
  i = 0;
  if (0 < count + -2) {
    pp = pose + 3;
    jp = joints + 4;
    do {
      pp[4] = *(ushort *)(jp + 0x30);
      pp[5] = *(ushort *)(jp + 0x34);
      pp[6] = *(ushort *)(jp + 0x38);
      _DAT_1f80038c = *pose;
      i = i + 1;
      _DAT_1f80038e = pp[-2];
      _DAT_1f800390 = pp[-1];
      w = *pp;
      pp = pp + 0xe;
      part_matrix_compute((undefined4 *)jp,parts,&DAT_1f80038c,(int)((uint)w << 0x10) >> 0x11);
      joints = joints + 0x50;
      pose = pose + 0xe;
      parts = parts + 0x20;
      jp = jp + 0x50;
    } while (i < count + -2);
  }
  rotmat_rtps_build(-(int)(short)pose[3] >> 1,(uint *)(joints + 4));
  rotmat_rtps_build(-(int)(short)pose[0x11] >> 1,(uint *)(joints + 0x54));
  return;

}

/* deferred_cmd_reset - clear deferred-cmd count and all ring
 * entries. */
void deferred_cmd_reset(void)
{
  undefined2 *ent;
  int i;
  
  _DAT_801f35f4 = 0;
  i = 0x1f;
  ent = (undefined2 *)&DAT_801f36ba;
  do {
    *ent = 0;
    i = i + -1;
    ent = ent + -3;
  } while (-1 < i);
  return;

}
/* anim_lerp_scalar - Q12 lerp of one channel: dst += (src-dst)*
 * alpha >> 12, with sign flip. */
void anim_lerp_scalar(short *dst,short src,int flip,int alpha)
{
  int d0;
  
  d0 = (int)src;
  if (flip != 0) {
    d0 = -d0;
  }
  alpha = (d0 - *dst) * alpha;
  if (alpha < 0) {
    alpha = alpha + 0xfff;
  }
  *dst = *dst + (short)(alpha >> 0xc);
  return;

}

/* anim_lerp_vec3 - vec3 channel lerp; y scaled by yscale>>7. */
void anim_lerp_vec3(short *dst,short *src,int flip,int alpha,short yscale)
{
  short sz;
  int dx;
  int dy;
  
  if (flip == 0) {
    dx = (int)*src;
  }
  else {
    dx = -(int)*src;
  }
  dy = (int)src[1];
  if (0 < dy) {
    dy = dy * yscale >> 7;
  }
  dx = (dx - *dst) * alpha;
  sz = src[2];
  if (dx < 0) {
    dx = dx + 0xfff;
  }
  dy = (dy - dst[1]) * alpha;
  *dst = *dst + (short)(dx >> 0xc);
  if (dy < 0) {
    dy = dy + 0xfff;
  }
  alpha = ((int)sz - (int)dst[2]) * alpha;
  dst[1] = dst[1] + (short)(dy >> 0xc);
  if (alpha < 0) {
    alpha = alpha + 0xfff;
  }
  dst[2] = dst[2] + (short)(alpha >> 0xc);
  return;

}

/* anim_lerp_vec3_raw - vec3 lerp without the y-scale. */
void anim_lerp_vec3_raw(short *dst,short *src,int flip,int alpha)
{
  short sz;
  int dx;
  int dy;
  
  if (flip == 0) {
    dy = (int)src[1];
  }
  else {
    dy = -(int)src[1];
  }
  dx = ((int)*src - (int)*dst) * alpha;
  sz = src[2];
  if (dx < 0) {
    dx = dx + 0xfff;
  }
  dy = (dy - dst[1]) * alpha;
  *dst = *dst + (short)(dx >> 0xc);
  if (dy < 0) {
    dy = dy + 0xfff;
  }
  alpha = ((int)sz - (int)dst[2]) * alpha;
  dst[1] = dst[1] + (short)(dy >> 0xc);
  if (alpha < 0) {
    alpha = alpha + 0xfff;
  }
  dst[2] = dst[2] + (short)(alpha >> 0xc);
  return;

}

/* anim_lerp_packed4 - packed 4-channel lerp (packed src words). */
void anim_lerp_packed4(short *dst,ushort *src,int flip,int alpha)
{
  uint pk;
  int d3;
  int d0;
  int d1;
  int d2;
  {
    extern void decomp_trace_record(uint8_t,uint32_t,uint32_t,uint8_t,uint32_t);
    uint32_t _gd = (uint32_t)guest_of(dst);
    if (_gd >= 0x801d0000u && _gd < 0x80200000u)
      decomp_trace_record(53, _gd, (uint32_t)guest_of(src), 4, 0);
  }
  
  pk = (uint)*src;
  d2 = pk + (uint)src[1] * 0x10000;
  if (flip == 0) {
    d3 = (int)(short)src[2];
    d0 = (int)(pk << 0x16) >> 0x11;
  }
  else {
    d0 = -((int)(pk << 0x16) >> 0x11);
    d3 = -(int)(short)src[2];
  }
  d0 = (d0 - *dst) * alpha;
  if (d0 < 0) {
    d0 = d0 + 0xfff;
  }
  d1 = ((d2 * 0x1000 >> 0x16) * 0x20 - (int)dst[1]) * alpha;
  *dst = *dst + (short)(d0 >> 0xc);
  if (d1 < 0) {
    d1 = d1 + 0xfff;
  }
  d2 = ((d2 * 4 >> 0x16) * 0x20 - (int)dst[2]) * alpha;
  dst[1] = dst[1] + (short)(d1 >> 0xc);
  if (d2 < 0) {
    d2 = d2 + 0xfff;
  }
  alpha = (d3 * 2 - (int)dst[3]) * alpha;
  dst[2] = dst[2] + (short)(d2 >> 0xc);
  if (alpha < 0) {
    alpha = alpha + 0xfff;
  }
  dst[3] = dst[3] + (short)(alpha >> 0xc);
  return;

}

/* anim_lerp_aux - aux channel lerp into dst+6. */
void anim_lerp_aux(byte *dst,int src,int flip,int alpha)
{
  int sv;
  
  sv = (src << 0x10) >> 0xf;
  if (flip != 0) {
    sv = -sv;
  }
  alpha = (sv - *(short *)(dst + 6)) * alpha;
  if (alpha < 0) {
    alpha = alpha + 0xfff;
  }
  *(short *)(dst + 6) = *(short *)(dst + 6) + (short)(alpha >> 0xc);
  return;

}

/* anim_lerp_packed3 - packed 3-channel lerp. */
void anim_lerp_packed3(short *dst,ushort *src,int flip,int alpha)
{
  ushort hi;
  int q;
  int d0;
  {
    extern void decomp_trace_record(uint8_t,uint32_t,uint32_t,uint8_t,uint32_t);
    uint32_t _gd = (uint32_t)guest_of(dst);
    if (_gd >= 0x801d0000u && _gd < 0x80200000u)
      decomp_trace_record(53, _gd, (uint32_t)guest_of(src), 3, 0);
  }
  
  q = (int)((uint)*src << 0x10) >> 0x18;
  d0 = q * 0x10;
  hi = src[1];
  if (flip != 0) {
    d0 = q * -0x10;
  }
  d0 = (d0 - *dst) * alpha;
  if (d0 < 0) {
    d0 = d0 + 0xfff;
  }
  q = (((int)((uint)hi << 0x18) >> 0x14) - (int)dst[1]) * alpha;
  *dst = *dst + (short)(d0 >> 0xc);
  if (q < 0) {
    q = q + 0xfff;
  }
  alpha = (((int)((uint)hi << 0x10) >> 0x18) * 0x10 - (int)dst[2]) * alpha;
  dst[1] = dst[1] + (short)(q >> 0xc);
  if (alpha < 0) {
    alpha = alpha + 0xfff;
  }
  dst[2] = dst[2] + (short)(alpha >> 0xc);
  return;

}


undefined8 vec2_rot_lut(undefined4 angle,int mag)
{
  int sn;
  int cs;
  
  sn = lut_sin_q12(angle);
  sn = mag * sn;
  cs = lut_cos_q12(angle);
  return CONCAT44((sn + 0x800) >> 0xc,(mag * -cs + 0x800) >> 0xc);

}

/* ---- event-stream runner + GTE helpers (batch21) ---- */
/* ev_stream_dispatch — event-stream dispatcher (guest-verbatim; a2-pc
 * residual modelled explicitly: handlers get pc = motion+0x84). */
void ev_stream_dispatch(byte *ctrl,byte *motion)
{
  byte *ret;
  extern uint32_t br2_call_generated(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
  extern uint32_t br2_gaddr(const void *p);
  /* Guest 0x8017ea7c carries the stream pc in $a2/$v0 through the
   * handler loop AND mirrors every nonzero next-pc into motion+0x84
   * (block_8017EAF0: sw v0,0x84(s0) @0x8017EAF4, executed on every
   * iteration before the next op fetch). Handlers that fall back to
   * `return motion+0x84` (EV5 action_switch paths) and the post-
   * dispatch resume point both depend on that store — without it
   * +0x84 stays at whatever action_switch/EV3 last wrote and every
   * resume/fallback reads a stale pc. Keep pc in a local guest
   * address for the a2-residual AND mirror it into +0x84 like the
   * guest does. */
  uint32_t pc_g = *(uint32_t *)(motion + 0x84);
  uint32_t _dbsp_n = 0;
  if (*(short *)(motion + 0x90) < 1) {
    for (;;) {
      uint8_t op = *(uint8_t *)guest_at(pc_g);
      uint32_t npc_g = 0;
      if (op < 24) {
        ret = (byte *)PTR_FUN_801b4e68[op]((uint8_t *)ctrl,(uint8_t *)motion, pc_g);
        npc_g = ret ? guest_of(ret) : 0;
      } else {
        /* Opcodes >=24 dispatch through the guest handler table; try the
         * clean-room port first (call_guest4 -> clean_try_dispatch), with
         * the same generated fallback br2_call_generated used. */
        uint32_t h = *(uint32_t *)guest_at(0x801b4e68u + (uint32_t)op * 4);
        extern uint32_t call_guest4(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
        npc_g = call_guest4(h, br2_gaddr(ctrl), br2_gaddr(motion), pc_g, 0);
        ret = npc_g ? (byte *)guest_at(npc_g) : (byte *)0;
      }
      { extern void decomp_trace_record(uint8_t,uint32_t,uint32_t,uint8_t,uint32_t);
        decomp_trace_record(60, pc_g, (uint32_t)br2_gaddr(motion), op, npc_g); }
      if (npc_g == 0) break;
      *(uint32_t *)(motion + 0x84) = npc_g;   /* guest sw @0x8017EAF4 */
      pc_g = npc_g;
      /* The generated body reaches debug_server_poll via
       * psx_check_interrupts_at at every block boundary (~16k checks);
       * this clean-C loop has no dispatch boundary, so a long or
       * non-terminating stream starves the debug socket permanently.
       * Poll periodically — host-side socket service only, no guest
       * interrupt/resume semantics. */
      if (((++_dbsp_n) & 0x3FFu) == 0) {
        extern void debug_server_poll(void);
        debug_server_poll();
      }
    }
  }
  else {
    *(short *)(motion + 0x90) = *(short *)(motion + 0x90) - *(short *)(motion + 0xce);
  }
  return;
}

/* part_matrix_compute - build a part rotation matrix from euler
 * angles + twist via GTE RTPS/RTPT ops; store to mat. */
void part_matrix_compute(undefined4 *mat,byte *parts,short *euler,undefined4 angle)
{
  uint m1;
  undefined2 m2;
  int ex;
  int ey;
  undefined4 m0;
  int ez;
  int t2;
  int t;
  int t3;
  undefined4 s0;
  undefined4 s1;
  undefined4 s2;
  undefined4 s3;
  undefined4 s4;
  undefined4 s5;
  undefined4 s6;
  int t1;
  undefined4 ra;
  ex = (int)*euler;
  ey = (int)euler[1];
  ez = (int)euler[2];
  _DAT_1f8003e0 = s6;
  _DAT_1f8003e4 = s5;
  _DAT_1f8003e8 = s4;
  _DAT_1f8003ec = s3;
  _DAT_1f8003f0 = s2;
  _DAT_1f8003f4 = s1;
  _DAT_1f8003f8 = s0;
  _DAT_1f8003fc = ra;
  FUN_801851d8();
  t1 = 0x1000 - ey;
  m0 = 0;
  t2 = ex;
  t3 = ez;
  FUN_801851d8();
  t = -t3 * t2;
  setCopReg(2,0x4800,-t3);
  setCopReg(2,0x5000,t2);
  setCopReg(2,0x5800,m0);
  _DAT_1f8003a0 = trig_cos(angle);
  copFunction(2,0xa00428);
  t2 = getCopReg(2,0xc800);
  t3 = getCopReg(2,0xd000);
  getCopReg(2,0xd800);
  setCopReg(2,0x4000,t1);
  setCopReg(2,0x4800,t2 >> 0xc);
  setCopReg(2,0x5000,t3 >> 0xc);
  setCopReg(2,0x5800,t >> 0xc);
  _DAT_1f8003b0 = (undefined2)_DAT_1f8003a0;
  _DAT_1f8003a8 = 0x1000;
  copFunction(2,0x190003d);
  _DAT_1f8003ac = trig_sin(angle);
  _DAT_1f8003a4 = -_DAT_1f8003ac & 0xffff;
  t = getCopReg(2,0xc800);
  t2 = getCopReg(2,0xd000);
  t3 = getCopReg(2,0xd800);
  _DAT_1f8003c8 = (short)ey;
  _DAT_1f8003d0 = (short)(t2 >> 0xc) + _DAT_1f8003c8;
  _DAT_1f8003c0 = (short)(t >> 0xc) + _DAT_1f8003c8;
  _DAT_1f8003c2 = (short)ex;
  _DAT_1f8003c4 = (undefined2)(t3 >> 0xc);
  _DAT_1f8003ce = (short)ez;
  _DAT_1f8003c6 = -_DAT_1f8003c2;
  _DAT_1f8003ca = -_DAT_1f8003ce;
  _DAT_1f8003cc = _DAT_1f8003c4;
  gte_mat_vec_swap(parts,&DAT_1f8003a0,&DAT_1f8003c0);
  m2 = _DAT_1f8003b0;
  t = _DAT_1f8003ac;
  m0 = _DAT_1f8003a8;
  m1 = _DAT_1f8003a4;
  *mat = _DAT_1f8003a0;
  mat[1] = m1;
  mat[2] = m0;
  mat[3] = t;
  *(undefined2 *)(mat + 4) = m2;
  return;

}

/* matrix_vec3_apply - apply the matrix to vec xyz via GTE ops;
 * returns vec. */
uint * matrix_vec3_apply(undefined4 *mat,uint *vec)
{
  uint r1;
  int r2;
  uint r3;
  int r4;
  uint r5;
  int r6;
  setCopControlWord(2,0,*mat);
  setCopControlWord(2,0x800,mat[1]);
  setCopControlWord(2,0x1000,mat[2]);
  setCopControlWord(2,0x1800,mat[3]);
  setCopControlWord(2,0x2000,mat[4]);
  setCopReg(2,0,(uint)(ushort)*vec | vec[1] & 0xffff0000);
  setCopReg(2,0x800,vec[3]);
  copFunction(2,0x486012);
  r1 = getCopReg(2,0x4800);
  r2 = getCopReg(2,0x5000);
  r3 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint)*(ushort *)((char *)vec + 2) | vec[2] << 0x10);
  setCopReg(2,0x800,(int)(short)*(ushort *)((char *)vec + 0xe));
  copFunction(2,0x486012);
  r4 = getCopReg(2,0x4800);
  r5 = getCopReg(2,0x5000);
  r6 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint)(ushort)vec[1] | vec[2] & 0xffff0000);
  setCopReg(2,0x800,vec[4]);
  copFunction(2,0x486012);
  *vec = r4 << 0x10 | r1 & 0xffff;
  vec[3] = r6 << 0x10 | r3 & 0xffff;
  r1 = getCopReg(2,0x4800);
  r4 = getCopReg(2,0x5000);
  vec[1] = r1 & 0xffff | r2 << 0x10;
  vec[2] = r4 << 0x10 | r5 & 0xffff;
  r1 = getCopReg(2,0xb);
  vec[4] = r1;
  return vec;

}


/* dir_rotate_acc/8d468 — verified against disasm 0x8018d444-0x8018d4f8.
 * Both tail-call dir_rotate_wrap (2D direction rotate+wrap, returns
 * quotients in $v1/$t4 packed into the u64 port's hi/lo halves).
 * 8d444: only when a1!=0, ACCUMULATES into out pair (a3).
 * 8d468: unconditional OVERWRITE of out pair. */
void dir_rotate_acc(int angle,int mag,byte *vec,byte *acc)
{
  if (mag != 0) {
    short x0 = *(short *)acc;
    short z0 = *(short *)(acc + 4);
    unsigned long long r = dir_rotate_wrap(angle,mag,(short *)vec);
    *(short *)acc = (short)(x0 + (short)(r >> 32));
    *(short *)(acc + 4) = (short)(z0 + (short)((uint32_t)r >> 16));
  }
}

void dir_rotate_set(int angle,int mag,byte *vec,byte *out)
{
  unsigned long long r = dir_rotate_wrap(angle,mag,(short *)vec);
  *(short *)out = (short)(r >> 32);
  *(short *)(out + 4) = (short)((uint32_t)r >> 16);
}
