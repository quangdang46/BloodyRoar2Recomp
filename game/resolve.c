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

int32_t action_resolve(byte *param_1, uint8_t *param_2)
{

  short sVar1;
  ushort uVar2;
  ushort uVar3;
  bool bVar4;
  ushort uVar5;
  byte bVar6;
  undefined2 uVar7;
  int iVar8;
  ushort uVar9;
  uint uVar10;
  uint uVar11;
  int iVar12;
  uint uVar13;
  uint uVar14;
  int iVar15;
  uint uVar16;
  int iVar17;
  uintptr_t iVar18;
  ushort uVar19;
  ushort uVar20;
  uint uVar21;
  
  iVar17 = -1;
  iVar15 = -1;
  uVar2 = *(ushort *)(param_1 + 0x7c);
  uVar14 = *(uint *)(param_1 + 0x68);
  iVar18 = (uintptr_t)guest_at(*(uint32_t *)(param_1 + 0x38));
  iVar12 = *(uint32_t *)(param_1 + 0x2c);
  uVar19 = uVar2 & 0xf000;
  uVar16 = *(uint *)(param_1 + 0x58);
  uVar21 = *(uint *)(param_1 + 0x5c);
  uVar11 = *(uint *)(param_1 + 0x60);
  uVar13 = *(uint *)(param_1 + 100);
  uVar10 = *(uint *)(param_1 + 0x6c);
  uVar9 = *(ushort *)(param_1 + 0x82) & 0xf000;
  if ((param_1[5] & 1) == 0) {
    uVar20 = *(ushort *)(param_1 + 0x80) & 0x4c8;
    uVar5 = *(ushort *)(param_1 + 0x84) & 0x4c8;
  }
  else {
    uVar20 = *(ushort *)(param_1 + 0x80) & 0x4e8;
    uVar5 = *(ushort *)(param_1 + 0x84) & 0x4e8;
  }
  uVar3 = *(ushort *)(param_1 + 0xce);
  if ((0 < *(short *)(param_1 + 0xce)) &&
     (*(short *)(param_1 + 0xce) = (short)((uint)uVar3 - (uint)*(ushort *)(param_1 + 0x88)),
     (int)(((uint)uVar3 - (uint)*(ushort *)(param_1 + 0x88)) * 0x10000) < 0)) {
    param_1[0xce] = 0;
    param_1[0xcf] = 0;
  }
  if ((uVar10 & 0x100000) != 0) {
    if (*(short *)(iVar18 + 0xd6) < 0x100) {
      *(undefined2 *)(iVar18 + 0xd6) = 0;
    }
    param_1[0x90] = 0;
    param_1[0x91] = 0;
    if (*(short *)(param_1 + 0x92) == 0) {
      param_1[0x92] = 0xa0;
      param_1[0x93] = 0;
    }
    param_1[0xb2] = 0;
    param_1[0xb3] = 0xec;
    param_1[0xb4] = 0x78;
    param_1[0xb5] = 0;
    uVar13 = *(uint *)(param_1 + 0x6c);
    uVar11 = uVar13 & 0xfff7ffff;
    *(short *)(param_1 + 0xc4) = *(short *)(param_1 + 0x4e) + *(short *)(param_1 + 0x70);
    *(uint *)(param_1 + 0x6c) = uVar11;
    if ((uVar10 & 0x80000) == 0) {
      *(uint *)(param_1 + 0x6c) = uVar13 & 0xffe7ffff;
      if ((uVar16 & 0x20000) != 0) {
        iVar17 = (int)*(short *)(param_1 + 0x70);
        if (iVar17 < 0) {
          if (-iVar17 < 0x400) {
            return 0x6d;
          }
          return 0x6e;
        }
        if (iVar17 < 0x400) {
          return 0x6d;
        }
        return 0x6e;
      }
      if ((uVar16 & 0x10000) == 0) {
        iVar17 = (int)*(short *)(param_1 + 0x70);
        if (iVar17 < 0) {
          if (-iVar17 < 0x400) {
            return 0x6b;
          }
          return 0x6c;
        }
        if (iVar17 < 0x400) {
          return 0x6b;
        }
        return 0x6c;
      }
      iVar17 = (int)*(short *)(param_1 + 0x70);
      if (iVar17 < 0) {
        if (-iVar17 < 0x400) {
          return 0x70;
        }
        return 0x6f;
      }
      if (iVar17 < 0x400) {
        return 0x70;
      }
      return 0x6f;
    }
    goto LAB_80176d24;
  }
  if ((uVar10 & 0x400000) != 0) {
    *(uint *)(param_1 + 0x6c) = *(uint *)(param_1 + 0x6c) & 0xffbfffff;
    sVar1 = *(short *)(iVar18 + 0x24);
    *(undefined2 *)(iVar18 + 0x24) = 0xffff;
    *(undefined2 *)(iVar18 + 0x2a) = 0xffff;
    pending_hit_a_clear(param_1);
    hit_chain_clear(param_1);
    return sVar1 + 0x90;
  }
  if ((uVar10 & 0x800000) != 0) {
    param_1[0xd4] = 0;
    param_1[0xd5] = 0;
    *(uint *)(param_1 + 0x6c) = *(uint *)(param_1 + 0x6c) & 0xff7fffff;
    if ((uVar10 & 0x8000) == 0) {
      pending_hit_a_clear(param_1);
      hit_chain_clear(param_1);
    }
    else {
      fighter_combat_reset(param_1);
    }
    *(undefined2 *)(&DAT_801f4d70 + (uint)param_1[2] * 0xbc) = 0;
    *(undefined2 *)(&DAT_801f4d72 + (uint)param_1[2] * 0xbc) = 0;
    *(undefined2 *)(&DAT_801f4d76 + (uint)param_1[2] * 0xbc) = 0xfff6;
    return *(short *)guest_at((uint32_t)(*(short *)(iVar18 + 0x2c) * 2 + *(int *)(iVar18 + 4) + -0x1a0)) + 0x90;
  }
  if ((uVar16 & 0x40000000) == 0) {
    if ((uVar16 & 0x100000) == 0) {
      if ((uVar21 & 0x4000000) != 0) goto LAB_801766e8;
      if ((uVar10 & 0x80000) != 0) {
        if ((uVar16 & 0x3000) == 0x3000) {
          if (((((*(ushort *)(param_1 + 0x7c) & 0x800) != 0) && ((uVar11 & 0x1000000) == 0)) &&
              ((uVar21 & 0x1400) == 0x1000)) && ((uVar2 & 0x2000) == 0)) {
            if ((*(ushort *)(param_1 + 0x7c) & 0x400) != 0) {
              *(uint *)(param_1 + 0x5c) = *(uint *)(param_1 + 0x5c) | 2;
            }
            *(uint *)(param_1 + 0x5c) = *(uint *)(param_1 + 0x5c) | 1;
          }
          *(uint *)(param_1 + 0x5c) = *(uint *)(param_1 + 0x5c) | 0x4000;
        }
        goto LAB_80176d18;
      }
      if (((uVar16 & 0x10000000) == 0) && ((uVar21 & 0x2000000) == 0)) {
        if (((uVar16 & 0x40000) == 0) && ((uVar16 & 0x40) == 0)) {
          if ((uVar16 & 0x6000000) == 0) {
            if (((uVar16 & 0x30000) == 0) || ((uVar11 & 0x20) != 0)) {
              if (((uVar16 & 0x8000) == 0) &&
                 ((((uVar16 & 0x80000) == 0 && ((uVar16 & 0x1000000) == 0)) && (-1 < (int)uVar16))))
              {
                if ((uVar16 & 0x1000) == 0) {
                  if ((uVar16 & 0x200) == 0) {
                    if ((uVar16 & 0x400) == 0) {
                      if ((uVar16 & 0x20) == 0) {
                        if ((uVar16 & 0x200000) != 0) {
                          return -1;
                        }
                        if ((uVar11 & 1) == 0) {
                          iVar15 = 1;
                          if (0 < *(short *)(param_1 + 0xce)) {
                            return 1;
                          }
                          uVar14 = *(uint *)(param_1 + 0x5c);
                          param_1[0xc0] = 0;
                          param_1[0xc1] = 0;
                          *(uint *)(param_1 + 0x5c) = uVar14 | 0x10000000;
                          if (((((*(ushort *)(param_1 + 0x7c) & 0x800) != 0) &&
                               ((uVar11 & 0x1000000) == 0)) && ((uVar21 & 0x1400) == 0x1000)) &&
                             ((uVar2 & 0x2000) == 0)) {
                            if ((*(ushort *)(param_1 + 0x7c) & 0x400) != 0) {
                              *(uint *)(param_1 + 0x5c) = uVar14 | 0x10000002;
                            }
                            *(uint *)(param_1 + 0x5c) = *(uint *)(param_1 + 0x5c) | 1;
                          }
                          *(uint *)(param_1 + 0x5c) = *(uint *)(param_1 + 0x5c) | 0x4000;
                          if (uVar19 == 0x6000) {
                            iVar15 = 0xd;
                          }
                          else if (uVar19 < 0x6001) {
                            if (uVar19 == 0x2000) {
                              iVar15 = 2;
                            }
                            else if (uVar19 == 0x4000) {
                              iVar15 = 0xb;
                            }
                          }
                          else if (uVar19 == 0x8000) {
                            if ((uVar21 & 0x100000) != 0) {
                              *(undefined2 *)(iVar18 + 0x82) = 0xffff;
                            }
                            iVar15 = 3;
                          }
                          else if (uVar19 == 0xc000) {
                            if ((uVar21 & 0x100000) != 0) {
                              *(undefined2 *)(iVar18 + 0x82) = 0xffff;
                            }
                            iVar15 = 0xe;
                          }
                          if ((*(ushort *)(param_1 + 0x7c) & 0x400) != 0) {
                            if (((*(int *)((char *)guest_at((uint32_t)iVar12) + 0x58) < 0) &&
                                ((int)*(short *)((char *)guest_at((uint32_t)iVar12) + 0xba) < *(short *)(param_1 + 0x88) * 10))
                               && ((*(short *)((char *)guest_at((uint32_t)iVar12) + 0xbc) < 1 &&
                                   (*(short *)(param_1 + 0x76) < 0x281)))) {
                              iVar15 = 0x3b;
                              if (((uVar2 & 0x4000) == 0) &&
                                 (iVar15 = 0x3a, (*(uint *)((char *)guest_at((uint32_t)iVar12) + 100) & 0x100) != 0)) {
                                iVar15 = 0x39;
                              }
                              if ((*(ushort *)(param_1 + 0x7c) & 8) != 0) {
                                return iVar15;
                              }
                            }
                            else if ((*(ushort *)(param_1 + 0x7c) & 8) != 0) {
                              *(undefined2 *)(iVar18 + 0x82) = 0xffff;
                              if ((uVar2 & 0x4000) == 0) {
                                return 0x39;
                              }
                              return 0x3b;
                            }
                          }
                          if ((uVar2 & 0x1000) != 0) {
                            return 0x13;
                          }
                          if (uVar9 == 0x4000) {
                            iVar15 = 0x11;
                          }
                          else if (uVar9 < 0x4001) {
                            if (uVar9 == 0x2000) {
                              iVar15 = 4;
                              *(undefined2 *)(iVar18 + 0x98) = 5;
                            }
                          }
                          else if (uVar9 == 0x8000) {
                            iVar15 = 7;
                          }
                          if ((*(uint *)((char *)guest_at(*(uint32_t *)(param_2 + 0xc4)) + 8) >> 10 & 1) != 0) {
                            if ((*(ushort *)(param_1 + 0x7a) & 1) != 0) {
                              return 0x1e;
                            }
                            if ((*(ushort *)(param_1 + 0x7a) & 2) != 0) {
                              return 0x1f;
                            }
                          }
                        }
                      }
                      else if ((uVar2 & 0x4000) == 0) {
                        iVar15 = 1;
                      }
                    }
                    else {
                      if ((uVar16 & 8) == 0) {
                        if (uVar19 != 0x2000) {
                          *(undefined2 *)(iVar18 + 0x98) = 6;
                        }
                      }
                      else {
                        if ((uVar16 & 2) != 0) {
                          if (uVar19 != 0x2000) {
                            if (uVar19 < 0x2001) {
                              iVar15 = 6;
                              if (uVar19 == 0x1000) {
                                iVar15 = 0x1a;
                              }
                            }
                            else if (uVar19 == 0x3000) {
                              iVar15 = 0x1b;
                            }
                            else {
                              iVar15 = 6;
                              if (uVar19 == 0x9000) {
                                iVar15 = 0x1c;
                              }
                            }
                          }
                          if ((uVar21 & 0x1800000) != 0) {
                            iVar15 = 6;
                          }
                        }
                        *(uint *)(param_1 + 0x5c) = *(uint *)(param_1 + 0x5c) | 0x4000;
                      }
                      if ((*(ushort *)(param_1 + 0x7a) & 8) != 0) {
                        iVar15 = 0x3c;
                      }
                      if (((*(short *)(iVar18 + 0x80) == 6) &&
                          ((*(ushort *)(param_1 + 0x7c) & 0x800) != 0)) &&
                         (((uVar11 & 0x1000000) == 0 &&
                          (((uVar21 & 0x1400) == 0x1000 && ((uVar2 & 0x2000) == 0)))))) {
                        if ((*(ushort *)(param_1 + 0x7c) & 0x400) != 0) {
                          *(uint *)(param_1 + 0x5c) = *(uint *)(param_1 + 0x5c) | 2;
                        }
                        uVar14 = *(uint *)(param_1 + 0x5c);
                        goto code_r0x80177a7c;
                      }
                    }
                  }
                  else {
                    uVar19 = *(ushort *)(param_1 + 0xcc);
                    if (*(short *)(param_1 + 0xcc) < 1) {
                      if (*(short *)(param_1 + 0xcc) == 0) goto LAB_80177940;
                    }
                    else {
                      *(short *)(param_1 + 0xcc) =
                           (short)((uint)uVar19 - (uint)*(ushort *)(param_1 + 0x88));
                      if ((int)(((uint)uVar19 - (uint)*(ushort *)(param_1 + 0x88)) * 0x10000) < 0) {
                        param_1[0xcc] = 0;
                        param_1[0xcd] = 0;
                      }
                    }
                  }
                }
                else if ((uVar16 & 0x400000) == 0) {
                  uVar14 = uVar16 & 0x2800;
                  if (uVar14 == 0x800) {
LAB_8017788c:
                    if ((uVar16 & 0x4000) == 0) {
                      if ((((*(ushort *)(param_1 + 0x7c) & 0x800) != 0) &&
                          ((uVar11 & 0x1000000) == 0)) &&
                         (((uVar21 & 0x1400) == 0x1000 && ((uVar2 & 0x2000) == 0)))) {
                        if ((*(ushort *)(param_1 + 0x7c) & 0x400) != 0) {
                          *(uint *)(param_1 + 0x5c) = *(uint *)(param_1 + 0x5c) | 2;
                        }
                        *(uint *)(param_1 + 0x5c) = *(uint *)(param_1 + 0x5c) | 1;
                      }
LAB_80177940:
                      *(uint *)(param_1 + 0x5c) = *(uint *)(param_1 + 0x5c) | 0x4000;
                    }
                  }
                  else if (0x800 < uVar14) {
                    if (uVar14 == 0x2000) {
                      if (((((*(ushort *)(param_1 + 0x7c) & 0x800) != 0) &&
                           ((uVar11 & 0x1000000) == 0)) && ((uVar21 & 0x1400) == 0x1000)) &&
                         ((uVar2 & 0x2000) == 0)) {
                        if ((*(ushort *)(param_1 + 0x7c) & 0x400) != 0) {
                          *(uint *)(param_1 + 0x5c) = *(uint *)(param_1 + 0x5c) | 2;
                        }
                        *(uint *)(param_1 + 0x5c) = *(uint *)(param_1 + 0x5c) | 1;
                      }
                      goto LAB_80177940;
                    }
                    if (uVar14 == 0x2800) goto LAB_8017788c;
                  }
                }
              }
            }
            else {
              if ((uVar16 & 0x8000000) != 0) {
                if ((uVar21 & 0x80080) != 0x80080) {
                  if (-1 < (int)uVar21) {
                    return -1;
                  }
                  if (*(int *)(_DAT_801f04a4 + 2) == 0) {
                    return -1;
                  }
                  iVar17 = (int)*(short *)(iVar18 + 0xd4);
                  iVar12 = (int)*(short *)(iVar18 + 0xd8);
                  if (*_DAT_801f04a4 == 0) {
                    iVar15 = _DAT_801f04a4[2] + -0x40;
                    iVar8 = _DAT_801f04a4[3] + -0x40;
                    if (((iVar15 < iVar17) && (0 < *(short *)(param_1 + 0x9a))) ||
                       ((iVar15 = -iVar15, iVar17 < iVar15 && (*(short *)(param_1 + 0x9a) < 0)))) {
                      *(short *)(iVar18 + 0xd4) = (short)iVar15;
                      param_1[0xb4] = 0;
                      param_1[0xb5] = 0;
                      param_1[0xb2] = 0;
                      param_1[0xb3] = 0;
                    }
                    if ((iVar8 < iVar12) && (0 < *(short *)(param_1 + 0x9e))) {
                      *(short *)(iVar18 + 0xd8) = (short)iVar8;
                    }
                    else {
                      if (-iVar8 <= iVar12) {
                        return -1;
                      }
                      if (-1 < *(short *)(param_1 + 0x9e)) {
                        return -1;
                      }
                      *(short *)(iVar18 + 0xd8) = (short)-iVar8;
                    }
                  }
                  else if (iVar17 * iVar17 + iVar12 * iVar12 <=
                           *(int *)(_DAT_801f04a4 + 2) + -0x1000) {
                    return -1;
                  }
                  param_1[0xb4] = 0;
                  param_1[0xb5] = 0;
                  param_1[0xb2] = 0;
                  param_1[0xb3] = 0;
                  return -1;
                }
                if ((uVar16 & 0x20000) != 0) {
                  iVar17 = (int)*(short *)(param_1 + 0x70);
                  if (iVar17 < 0) {
                    if (-iVar17 < 0x400) {
                      return 0x6d;
                    }
                    return 0x6e;
                  }
                  if (iVar17 < 0x400) {
                    return 0x6d;
                  }
                  return 0x6e;
                }
                if ((uVar16 & 0x10000) == 0) {
                  iVar17 = (int)*(short *)(param_1 + 0x70);
                  if (iVar17 < 0) {
                    if (-iVar17 < 0x400) {
                      return 0x6b;
                    }
                    return 0x6c;
                  }
                  if (iVar17 < 0x400) {
                    return 0x6b;
                  }
                  return 0x6c;
                }
                iVar17 = (int)*(short *)(param_1 + 0x70);
                if (iVar17 < 0) {
                  if (-iVar17 < 0x400) {
                    return 0x70;
                  }
                  return 0x6f;
                }
                if (iVar17 < 0x400) {
                  return 0x70;
                }
                return 0x6f;
              }
              *(uint *)(param_1 + 0x5c) = *(uint *)(param_1 + 0x5c) | 0x8000;
              uVar9 = *(ushort *)(param_1 + 0xc0);
              if (*(short *)(param_1 + 0xc0) < 1) {
                if (*(short *)(param_1 + 0xc0) == 0) {
                  if ((*(ushort *)(param_1 + 0x7c) & 0x200) != 0) {
                    uVar19 = uVar2 & 0xa000;
                    if ((uVar2 & 0x1000) != 0) {
                      uVar19 = uVar19 | 0x4000;
                    }
                    if ((uVar2 & 0x4000) != 0) {
                      uVar19 = uVar19 | 0x1000;
                    }
                  }
                  if ((uVar16 & 0x20000) == 0) {
                    if ((uVar10 & 0x10000) == 0) {
                      if (uVar19 == 0x2000) {
                        iVar15 = 0x79;
                      }
                      else if (uVar19 < 0x2001) {
                        iVar15 = 0x77;
                        if (uVar19 == 0x1000) {
                          iVar15 = 0x7c;
                        }
                      }
                      else if (uVar19 == 0x4000) {
                        iVar15 = 0x7b;
                      }
                      else {
                        iVar15 = 0x77;
                        if (uVar19 == 0x8000) {
                          iVar15 = 0x7a;
                        }
                      }
                    }
                    else {
                      iVar17 = 0x1f;
                    }
                  }
                  else if ((uVar10 & 0x10000) == 0) {
                    if (uVar19 == 0x2000) {
                      iVar15 = 0x73;
                    }
                    else if (uVar19 < 0x2001) {
                      iVar15 = 0x71;
                      if (uVar19 == 0x1000) {
                        iVar15 = 0x76;
                      }
                    }
                    else if (uVar19 == 0x4000) {
                      iVar15 = 0x75;
                    }
                    else {
                      iVar15 = 0x71;
                      if (uVar19 == 0x8000) {
                        iVar15 = 0x74;
                      }
                    }
                  }
                  else {
                    iVar17 = 0x1e;
                  }
                }
              }
              else {
                *(short *)(param_1 + 0xc0) =
                     (short)((uint)uVar9 - (uint)*(ushort *)(param_1 + 0x88));
                if ((int)(((uint)uVar9 - (uint)*(ushort *)(param_1 + 0x88)) * 0x10000) < 0) {
                  param_1[0xc0] = 0;
                  param_1[0xc1] = 0;
                }
                if ((*(ushort *)(param_1 + 0x7a) & 0xf0e0) == 0) {
LAB_8017764c:
                  sVar1 = *(short *)(param_1 + 0xc0);
                }
                else {
                  uVar19 = *(ushort *)(param_1 + 0xc0);
                  sVar1 = 0;
                  if (*(short *)(param_1 + 0xc0) != 0) {
                    *(short *)(param_1 + 0xc0) =
                         (short)((uint)uVar19 - (uint)*(ushort *)(param_1 + 0x88));
                    if ((int)(((uint)uVar19 - (uint)*(ushort *)(param_1 + 0x88)) * 0x10000) < 0) {
                      param_1[0xc0] = 0;
                      param_1[0xc1] = 0;
                    }
                    goto LAB_8017764c;
                  }
                }
                if ((sVar1 < 0x80) && ((*(ushort *)(param_1 + 0x82) & 0x40) != 0)) {
                  *(uint *)(param_1 + 0x6c) = *(uint *)(param_1 + 0x6c) | 0x10000;
                }
              }
            }
          }
          else {
            if ((uVar16 & 0x800) == 0) {
              return -1;
            }
            if (0 < *(short *)(param_1 + 0x90)) {
              if (((int)*(short *)(param_1 + 0x94) < *(int *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0x268)) ||
                 ((int)*(short *)(param_1 + 0x94) < *(int *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0x358))) {
                if ((uVar16 & 0x4000000) != 0) {
                  return (int)*(short *)(iVar18 + 0x9a);
                }
                return 0x5a;
              }
            }
            uVar19 = *(ushort *)(param_1 + 0xcc);
            if ((0 < *(short *)(param_1 + 0xcc)) &&
               (*(short *)(param_1 + 0xcc) =
                     (short)((uint)uVar19 - (uint)*(ushort *)(param_1 + 0x88)),
               (int)(((uint)uVar19 - (uint)*(ushort *)(param_1 + 0x88)) * 0x10000) < 0)) {
              param_1[0xcc] = 0;
              param_1[0xcd] = 0;
            }
            if ((int)uVar21 < 0) {
              if (*(int *)(_DAT_801f04a4 + 2) != 0) {
                iVar12 = *(int *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0x174);
                iVar8 = *(int *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0x17c);
                if (*_DAT_801f04a4 == 0) {
                  if ((((_DAT_801f04a4[2] + -0x40 < iVar12) && (0 < *(short *)(param_1 + 0x9a))) ||
                      ((iVar12 < -(_DAT_801f04a4[2] + -0x40) && (*(short *)(param_1 + 0x9a) < 0))))
                     || (((_DAT_801f04a4[3] + -0x40 < iVar8 && (0 < *(short *)(param_1 + 0x9e))) ||
                         ((iVar8 < -(_DAT_801f04a4[3] + -0x40) && (*(short *)(param_1 + 0x9e) < 0)))
                         ))) {
LAB_801770c0:
                    if (*(short *)(param_1 + 0x94) + -0x100 <=
                        *(int *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0x178)) goto LAB_801772e0;
                    iVar15 = 0x4c;
                    if (*(short *)(param_1 + 0x96) == 0x800) {
                      iVar15 = 0x4b;
                    }
                  }
                }
                else if (*(int *)(_DAT_801f04a4 + 2) + -0x1000 < iVar12 * iVar12 + iVar8 * iVar8)
                goto LAB_801770c0;
              }
            }
            else if ((uVar16 & 0x4000000) == 0) {
              if (*(short *)(param_1 + 0xb2) == 0) {
                iVar12 = (int)*(short *)(param_1 + 0x98);
                if (iVar12 < 0) {
                  iVar12 = -iVar12;
                }
                if (iVar12 < 0x400) goto LAB_801772ec;
              }
              if (((uVar21 & 0x1000000) != 0) &&
                 (iVar15 = 0x8a, *(int *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0x178) < -0x100)) {
LAB_801772c8:
                iVar15 = 0x62;
                if (*(short *)(param_1 + 0x96) == 0x800) {
                  iVar15 = 0x61;
                }
              }
            }
            else if ((uVar21 & 0x1000000) != 0) {
              if (((*param_1 & 0x80) == 0) ||
                 (param_1[0xe5] < *(byte *)((char *)guest_at(*(uint32_t *)(param_2 + 0xc0)) + 4))) {
                if (*(int *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0x178) < -0x100) {
                  uVar7 = (undefined2)
                          ((uint)(((int)*(short *)(param_1 + 0x24) / 10) * 0x10000) >> 0x10);
                  if ((uVar10 & 1) != 0) {
                    _DAT_801f04f4 = 0xffff;
                    _DAT_801f04ec = uVar7;
                  }
                  if ((uVar10 & 2) != 0) {
                    _DAT_801f04f6 = 0xffff;
                    _DAT_801f04ee = uVar7;
                  }
                  if ((uVar10 & 4) != 0) {
                    _DAT_801f04f8 = 0xffff;
                    _DAT_801f04f0 = uVar7;
                  }
                  if ((uVar10 & 8) != 0) {
                    _DAT_801f04fa = 0xffff;
                    _DAT_801f04f2 = uVar7;
                  }
                  goto LAB_801772c8;
                }
LAB_801772e0:
                iVar15 = 0x8a;
              }
              else {
                DAT_801f04c9 = 0;
                if ((uVar10 & 1) != 0) {
                  _DAT_801f04ec = 10000;
                  _DAT_801f04f4 = 0xffff;
                }
                if ((uVar10 & 2) != 0) {
                  _DAT_801f04ee = 10000;
                  _DAT_801f04f6 = 0xffff;
                }
                if ((uVar10 & 4) != 0) {
                  _DAT_801f04f0 = 10000;
                  _DAT_801f04f8 = 0xffff;
                }
                if ((uVar10 & 8) != 0) {
                  _DAT_801f04f2 = 10000;
                  _DAT_801f04fa = 0xffff;
                }
              }
            }
LAB_801772ec:
            if (((uVar20 & 0xc0) == 0xc0) && (*(short *)(param_1 + 0xcc) == 0)) {
              param_1[0xc0] = 0;
              param_1[0xc1] = 0;
              if (*(int *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0x178) - (int)*(short *)(param_1 + 0x94) <
                  -0x98) {
                if ((uVar2 & 0x1000) == 0) {
                  if ((uVar2 & 0x4000) == 0) {
                    param_1[0x90] = 0;
                    param_1[0x91] = 0;
                  }
                  else {
                    param_1[0x90] = 0x80;
                    param_1[0x91] = 2;
                  }
                }
                else {
                  param_1[0x90] = 0;
                  param_1[0x91] = 0xfb;
                }
                iVar15 = 0x3d;
                param_1[0x92] = 0xa0;
                param_1[0x93] = 0;
                if ((uVar2 & 0x2000) == 0) {
                  if ((uVar2 & 0x8000) == 0) goto LAB_80177350;
                  param_1[0x98] = 0;
                  param_1[0x99] = 0xfe;
                }
                else {
                  param_1[0x98] = 0;
                  param_1[0x99] = 2;
                }
              }
              else {
                param_1[0xb4] = 0x78;
                param_1[0xb5] = 0;
                sVar1 = *(short *)(param_1 + 0x98);
                iVar15 = 0x3e;
                if (sVar1 < 0) {
                  sVar1 = -sVar1;
                }
                *(short *)(param_1 + 0xb2) = *(short *)(param_1 + 0xb2) + sVar1;
LAB_80177350:
                param_1[0x98] = 0;
                param_1[0x99] = 0;
              }
            }
          }
        }
      }
      else if ((int)uVar16 < 0) {
        uVar14 = *(uint *)(param_1 + 0x5c) & 0xfdfffffb;
code_r0x80177a7c:
        *(uint *)(param_1 + 0x5c) = uVar14 | 1;
      }
      else {
        if ((uVar21 & 0x2000000) != 0) {
          *(undefined2 *)(iVar18 + 0x80) = 0x30;
          *(undefined2 *)(iVar18 + 0x82) = 0xffff;
          param_1[0xcc] = 0;
          param_1[0xcd] = 0;
          if ((uVar16 & 0x800) == 0) {
            if ((uVar21 & 4) == 0) {
              iVar15 = 0x32;
              if (((uVar16 & 0x10) == 0) && (iVar15 = 0x31, (uVar14 & 0x400000) != 0)) {
                iVar15 = 0x30;
              }
            }
            else {
              iVar15 = 0x38;
              if (((uVar16 & 0x10) == 0) && (iVar15 = 0x37, (uVar14 & 0x400000) != 0)) {
                iVar15 = 0x36;
              }
            }
          }
          else {
            iVar15 = 0x33;
            if ((uVar21 & 4) != 0) {
              iVar15 = 0x34;
            }
          }
        }
        uVar14 = *(uint *)(param_1 + 0x5c);
        sVar1 = *(short *)(param_1 + 0xcc);
        *(short *)(param_1 + 0xcc) = sVar1 + *(short *)(param_1 + 0x88);
        *(uint *)(param_1 + 0x5c) = uVar14 & 0xfdfffffb;
        if ((short)(sVar1 + *(short *)(param_1 + 0x88)) < 0x61) {
          if ((uVar21 & 0x80) == 0) {
            if (((uVar21 & 0x2004) == 0) && ((*(ushort *)(param_1 + 0x7a) & 0x2000) != 0)) {
              uVar16 = uVar14 & 0xfdfffffb | 1;
              iVar12 = 0x35;
              goto LAB_80176f0c;
            }
          }
          else if (((uVar21 & 0x2004) == 0) &&
                  (iVar12 = 0x3f, (*(ushort *)(param_1 + 0x7a) & 0x2000) != 0)) {
            uVar16 = *(uint *)(param_1 + 0x5c) | 1;
            goto LAB_80176f0c;
          }
        }
        if ((*(short *)(iVar18 + 0x80) != 0x35) && (*(short *)(iVar18 + 0x80) != 0x3f)) {
          if ((*(ushort *)(param_1 + 0x7c) & 0x800) == 0) {
            return iVar15;
          }
          if ((uVar11 & 0x1000000) != 0) {
            return iVar15;
          }
          if ((uVar21 & 0x1400) != 0x1000) {
            return iVar15;
          }
          if ((uVar2 & 0x2000) == 0) {
            if ((*(ushort *)(param_1 + 0x7c) & 0x400) != 0) {
              *(uint *)(param_1 + 0x5c) = *(uint *)(param_1 + 0x5c) | 2;
            }
            *(uint *)(param_1 + 0x5c) = *(uint *)(param_1 + 0x5c) | 1;
            return iVar15;
          }
          return iVar15;
        }
        if ((*(uint *)(param_1 + 0x60) & 0x4000000) != 0) {
          uVar16 = *(uint *)(param_1 + 0x5c) | 1;
          iVar12 = iVar15;
LAB_80176f0c:
          *(uint *)(param_1 + 0x5c) = uVar16;
          return iVar12;
        }
      }
      if ((uVar11 & 0x10000) != 0) {
        if (iVar17 < 0) {
          return iVar15;
        }
        return iVar17 + 0x90;
      }
      if (0 < *(short *)(param_1 + 0xce)) {
        return iVar15;
      }
      bVar4 = false;
      if ((((int)uVar16 < 0) && (*(short *)(param_1 + 0xbc) == *(short *)(param_1 + 0x88))) &&
         (((uVar13 & 0x80000000) != 0 || ((*(uint *)((char *)guest_at(*(uint32_t *)(param_2 + 0xc4)) + 8) >> 0xd & 1) != 0)
          ))) {
        bVar4 = true;
      }
      if (((((uVar16 & 0x40000) != 0) || ((uVar16 & 0x40) != 0)) || ((uVar16 & 0x8000) != 0)) ||
         (((uVar16 & 0x80080000) == 0x80000 || ((uVar16 & 0x1000000) != 0)))) goto LAB_8017862c;
      if ((int)uVar16 < 0) {
        if (bVar4) {
          if (((*(ushort *)(param_1 + 0x80) & 0xe8) == 0xe0) && ((param_1[5] & 0x10) == 0)) {
LAB_8017857c:
            pending_hit_a_clear(param_1);
            hit_chain_clear(param_1);
            if (*(short *)(param_1 + 0xd6) < 0x1e00) {
              return 0xbd;
            }
            if (param_1[1] == 0) {
              if ((DAT_801b55d0 & 1) != 0) {
                (&DAT_801f596c)[(uint)param_1[2] * 2] = 0x3c;
                (&DAT_801f5970)[(uint)param_1[2] * 2] = 0xff;
                (&DAT_801f596d)[(uint)param_1[2] * 2] = 0x3c;
                (&DAT_801f5971)[(uint)param_1[2] * 2] = 1;
              }
              return 0xbe;
            }
            return 0xbe;
          }
          if (((-1 < *(short *)(iVar18 + 0x1c)) && ((*(uint *)(iVar18 + 0x18) & 0x20) != 0)) &&
             (iVar17 = pending_action_gate(guest_of((void *)iVar18),param_1,uVar21,uVar16), -1 < iVar17)) {
            return iVar17;
          }
        }
        uVar11 = (uint)*(byte *)(iVar18 + 0x7c);
        if (((uVar11 != *(byte *)(iVar18 + 0x7d)) && (0 < *(short *)(param_1 + 0xbc))) &&
           (iVar12 = (int)(guest_of((const void *)iVar18) + uVar11 * 2),
            *(short *)((char *)guest_at((uint32_t)iVar12) + 0x5c) <= *(short *)(param_1 + 0xbc)))
        {
          sVar1 = *(short *)((char *)guest_at((uint32_t)iVar12) + 0x3c);
          if (uVar11 == (int)*(char *)(iVar18 + 0x7e)) {
            *(undefined1 *)(iVar18 + 0x7e) = 0xff;
          }
          bVar6 = *(char *)(iVar18 + 0x7c) + 1;
          *(byte *)(iVar18 + 0x7c) = bVar6 - (bVar6 & 0xf0);
          *(undefined2 *)(iVar18 + 0x82) = 0xffff;
          return sVar1 + 0x1090;
        }
        if (((0 < *(short *)(param_1 + 0xba)) && ((uVar21 & 0x40080) == 0)) &&
           (((uVar13 & 0x40002000) == 0x40002000 && ((*(ushort *)(param_1 + 0x7a) & 8) != 0)))) {
          if ((uVar10 & 0x8000) == 0) {
            hit_chain_clear(param_1);
            pending_hit_a_clear(param_1);
            pending_hit_b_clear(param_1);
          }
          else {
            fighter_combat_reset(param_1);
          }
          if ((uVar16 & 0x10) == 0) {
            iVar15 = 1;
            slot_mark2_set(param_1[2]);
            *(undefined2 *)(&DAT_801f4d70 + (uint)param_1[2] * 0xbc) = 0;
            *(undefined2 *)(&DAT_801f4d72 + (uint)param_1[2] * 0xbc) = 0;
            bVar6 = param_1[2];
          }
          else {
            iVar15 = 0xb;
            slot_mark2_set(param_1[2]);
            *(undefined2 *)(&DAT_801f4d70 + (uint)param_1[2] * 0xbc) = 0;
            *(undefined2 *)(&DAT_801f4d72 + (uint)param_1[2] * 0xbc) = 0;
            bVar6 = param_1[2];
          }
          *(undefined2 *)(&DAT_801f4d76 + (uint)bVar6 * 0xbc) = 0xffe0;
          param_1[0xce] = 0xa0;
          param_1[0xcf] = 0;
        }
        goto LAB_8017862c;
      }
      if ((uVar16 & 0x1000) == 0) {
        if ((uVar16 & 0x400) == 0) {
          if ((uVar16 & 0x20) != 0) {
            if ((uVar20 & 0x80) == 0) {
              if ((uVar20 & 0x40) == 0) {
                if ((uVar20 & 0x20) != 0) {
                  iVar17 = 0x2a;
                }
              }
              else {
                iVar17 = 0x1a;
              }
            }
            else {
              iVar17 = 10;
            }
            goto LAB_8017862c;
          }
        }
        else {
          if ((uVar16 & 8) == 0) {
            if ((uVar16 & 2) != 0) {
              if ((uVar20 & 0x80) == 0) {
                if ((uVar20 & 0x40) == 0) {
                  if ((uVar20 & 0x20) != 0) {
                    iVar17 = 0x26;
                  }
                }
                else {
                  iVar17 = 0x16;
                }
              }
              else {
                iVar17 = 6;
              }
            }
            goto LAB_8017862c;
          }
          if (((uVar16 & 2) != 0) && ((uVar2 & 0x2000) != 0)) {
            if ((uVar20 & 0x80) == 0) {
              if ((uVar20 & 0x40) == 0) {
                if (((*(ushort *)(param_1 + 0x80) & 0x20) != 0) &&
                   (((param_1[5] & 0x10) != 0 || (0x1dff < *(short *)(param_1 + 0xd6))))) {
                  iVar17 = 0x27;
                }
              }
              else {
                iVar17 = 0x17;
              }
            }
            else {
              iVar17 = 7;
            }
            goto LAB_8017862c;
          }
        }
        if ((-1 < *(short *)(iVar18 + 0x1c)) &&
           (iVar17 = pending_action_gate(guest_of((void *)iVar18),param_1,uVar21,uVar16), -1 < iVar17)) {
          return iVar17;
        }
        if ((uVar2 & 0x4000) == 0) {
          if ((uVar21 & 0x400000) == 0) {
            if ((uVar2 & 0x2000) == 0) {
              if ((uVar2 & 0x8000) == 0) {
                if ((uVar20 & 0x80) == 0) {
                  if ((uVar20 & 0x40) == 0) {
                    if ((uVar20 & 0x20) != 0) {
                      iVar17 = 0x20;
                    }
                  }
                  else {
                    iVar17 = 0x10;
                  }
                }
                else {
                  iVar17 = 0;
                }
              }
              else if ((uVar20 & 0x80) == 0) {
                if ((uVar20 & 0x40) == 0) {
                  if ((uVar20 & 0x20) != 0) {
                    iVar17 = 0x22;
                  }
                }
                else {
                  iVar17 = 0x12;
                }
              }
              else {
                iVar17 = 2;
              }
            }
            else if ((uVar20 & 0x80) == 0) {
              if ((uVar20 & 0x40) == 0) {
                if ((uVar20 & 0x20) != 0) {
                  iVar17 = 0x21;
                }
              }
              else {
                iVar17 = 0x11;
              }
            }
            else {
              iVar17 = 1;
            }
          }
          else if ((uVar20 & 0x80) == 0) {
            if ((uVar20 & 0x40) == 0) {
              if ((uVar20 & 0x20) != 0) {
                iVar17 = 0x2b;
              }
            }
            else {
              iVar17 = 0x1b;
            }
          }
          else {
            iVar17 = 0xb;
          }
        }
        else {
          if ((uVar21 & 0x400000) == 0) {
            if ((uVar2 & 0x2000) == 0) {
              if ((uVar2 & 0x8000) == 0) {
                if ((uVar20 & 0x80) == 0) {
                  if ((uVar20 & 0x40) == 0) {
                    if ((uVar20 & 0x20) != 0) {
                      iVar17 = 0x23;
                    }
                  }
                  else {
                    iVar17 = 0x13;
                  }
                }
                else {
                  iVar17 = 3;
                }
              }
              else if ((uVar20 & 0x80) == 0) {
                if ((uVar20 & 0x40) == 0) {
                  if ((uVar20 & 0x20) != 0) {
                    iVar17 = 0x25;
                  }
                }
                else {
                  iVar17 = 0x15;
                }
              }
              else {
                iVar17 = 5;
              }
            }
            else if ((uVar20 & 0x80) == 0) {
              if ((uVar20 & 0x40) == 0) {
                if ((uVar20 & 0x20) != 0) {
                  iVar17 = 0x24;
                }
              }
              else {
                iVar17 = 0x14;
              }
            }
            else {
              iVar17 = 4;
            }
          }
          else if ((uVar20 & 0x80) == 0) {
            if ((uVar20 & 0x40) == 0) {
              if ((uVar20 & 0x20) != 0) {
                iVar17 = 0x2c;
              }
            }
            else {
              iVar17 = 0x1c;
            }
          }
          else {
            iVar17 = 0xc;
          }
          if (((uVar21 & 0x800) != 0) && ((*(ushort *)(param_1 + 0x80) & 0x4000) != 0)) {
            if ((uVar20 & 0x80) == 0) {
              if ((uVar20 & 0x40) == 0) {
                if ((uVar20 & 0x20) != 0) {
                  iVar17 = 0x32;
                }
              }
              else {
                iVar17 = 0x31;
              }
            }
            else {
              iVar17 = 0x30;
            }
          }
        }
        if (((*(ushort *)(param_1 + 0x80) & 0x20) != 0) && ((param_1[5] & 0x10) == 0))
        goto LAB_8017857c;
        goto LAB_8017862c;
      }
      if ((uVar16 & 0x400000) != 0) goto LAB_8017862c;
      if ((*(uint *)(iVar18 + 0x30) & 0x20000) == 0) {
        if ((*(uint *)(iVar18 + 0x30) & 0x40000) == 0) {
          if ((*(int *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0x178) < (int)*(short *)(iVar18 + 0x38)) &&
             ((uVar16 & 0x800) != 0)) goto LAB_80178118;
        }
        else if ((uVar16 & 0x2800) == 0x2000) goto LAB_80178120;
      }
      else if ((uVar16 & 0x2800) == 0) {
LAB_80178118:
        iVar17 = (int)*(short *)(iVar18 + 0x34);
LAB_80178120:
        *(undefined2 *)(iVar18 + 0x34) = 0xffff;
      }
      uVar16 = uVar16 & 0x2800;
      if ((((uVar16 == 0x800) || (((0x800 < uVar16 && (uVar16 != 0x2000)) && (uVar16 == 0x2800))))
          && ((((*(ushort *)(param_1 + 0x80) & 0x20) != 0 && (0x1dff < *(short *)(param_1 + 0xd6)))
              && ((param_1[5] & 0x10) == 0)))) &&
         ((iVar17 = 0x2e, param_1[1] == 0 && ((DAT_801b55d0 & 1) != 0)))) {
        (&DAT_801f596c)[(uint)param_1[2] * 2] = 0x3c;
        (&DAT_801f5970)[(uint)param_1[2] * 2] = 0xff;
        (&DAT_801f596d)[(uint)param_1[2] * 2] = 0x3c;
        (&DAT_801f5971)[(uint)param_1[2] * 2] = 1;
      }
LAB_8017862c:
      if (iVar17 < 0) {
        return iVar15;
      }
      return iVar17 + 0x90;
    }
    sVar1 = *(short *)(param_1 + 0xd4);
    if ((*(short *)(param_1 + 0xd4) != 0) &&
       (*(ushort *)(param_1 + 0xd4) = sVar1 - 0x10U,
       (int)((uint)(ushort)(sVar1 - 0x10U) << 0x10) < 0)) {
      param_1[0xd4] = 0;
      param_1[0xd5] = 0;
    }
    if ((uVar10 & 0x4000000) != 0) {
      *(uint *)(param_1 + 0x6c) = *(uint *)(param_1 + 0x6c) & 0xfbffffff;
      *(short *)(param_1 + 0xce) = *(short *)(param_1 + 0x88) * 0x26;
      pending_hit_a_clear(param_1);
      pending_hit_b_clear(param_1);
      return 0x7e;
    }
    if ((uVar10 & 0x80000) == 0) goto LAB_801766c8;
  }
  else {
    if ((uVar10 & 0x4000000) != 0) {
      *(uint *)(param_1 + 0x6c) = *(uint *)(param_1 + 0x6c) & 0xfbffffff;
      *(short *)(param_1 + 0xce) = *(short *)(param_1 + 0x88) * 0x26;
      pending_hit_a_clear(param_1);
      pending_hit_b_clear(param_1);
      return 0x7f;
    }
    if ((uVar10 & 0x80000) == 0) {
      if ((uVar21 & 0x4000000) == 0) {
LAB_801766c8:
        *(uint *)(param_1 + 0x5c) = *(uint *)(param_1 + 0x5c) & 0xfbffffff;
        return -1;
      }
LAB_801766e8:
      *(uint *)(param_1 + 0x5c) = *(uint *)(param_1 + 0x5c) & 0xfbffffff;
      *(uint *)(param_1 + 0x6c) = *(uint *)(param_1 + 0x6c) & 0xfff7efff;
      *(undefined2 *)(iVar18 + 0x82) = 0xffff;
      slot_mark2_set(param_1[2]);
      if ((uVar10 & 0x8000) == 0) {
        pending_hit_a_clear(param_1);
      }
      else {
        fighter_combat_reset(param_1);
      }
      *(undefined2 *)(&DAT_801f4d70 + (uint)param_1[2] * 0xbc) = 0;
      *(undefined2 *)(&DAT_801f4d72 + (uint)param_1[2] * 0xbc) = 0;
      *(undefined2 *)(&DAT_801f4d76 + (uint)param_1[2] * 0xbc) = 0xff00;
      if ((uVar14 & 0x2000000) != 0) {
        if ((uVar16 & 0x20000) != 0) {
          return 0x69;
        }
        if ((uVar16 & 0x10000) != 0) {
          return 0x6a;
        }
      }
      if ((*param_1 & 0x80) != 0) {
        *(uint *)(param_1 + 0x5c) = *(uint *)(param_1 + 0x5c) & 0xffdfffff;
        if (((uVar14 & 0x4000) == 0) && ((uVar14 & 0x800) == 0)) {
          if ((uVar14 & 0x10000) == 0) {
            if ((uVar14 & 0x2000) == 0) {
              if ((uVar14 & 0x40000) == 0) {
                *(uint *)(param_1 + 0x68) = *(uint *)(param_1 + 0x68) & 0xffffefff | 0x804400;
                trig_lut_read(iVar12,param_1);
                if ((param_1[5] & 1) == 0) {
                  return 0x5b;
                }
                if ((param_1[0xe5] < *(byte *)((char *)guest_at(*(uint32_t *)(param_2 + 0xc0)) + 4)) &&
                   (-1 < *(short *)(param_1 + 0xd6))) {
                  return 0x5b;
                }
                fighter_combat_reset(param_1);
                return 0x5b;
              }
              uVar14 = uVar14 | 0x800000;
              *(uint *)(param_1 + 0x68) = uVar14;
              trig_vel_diag(iVar12,param_1);
            }
            else {
              uVar14 = uVar14 | 0x800000;
              *(uint *)(param_1 + 0x68) = uVar14;
              trig_vel_up(iVar12,param_1);
            }
          }
          else {
            uVar14 = uVar14 | 0x800000;
            *(uint *)(param_1 + 0x68) = uVar14;
            trig_vel_flat(iVar12,param_1);
          }
        }
        else {
          uVar14 = uVar14 | 0x800000;
          *(uint *)(param_1 + 0x68) = uVar14;
          trig_lut_read(iVar12,param_1);
        }
      }
      if ((((uVar14 & 0x2000000) != 0) ||
          ((((uVar14 & 0x800000) != 0 && ((uVar14 & 0x56800) != 0)) || ((uVar14 & 0x2000) != 0))))
         && (((param_1[5] & 1) != 0 && (*(short *)(param_1 + 0xd6) < 0)))) {
        fighter_combat_reset(param_1);
      }
      if (((uVar14 & 3) == 2) && ((uVar5 & 0x400) == 0)) {
        iVar15 = 0x45;
      }
      else if ((uVar16 & 0x30) == 0) {
        if ((uVar14 & 0x100000) == 0) {
          if ((uVar14 & 0x200000) == 0) {
            if ((uVar14 & 0x400000) != 0) {
              iVar15 = 0x40;
            }
          }
          else {
            iVar15 = 0x41;
          }
        }
        else {
          iVar15 = 0x42;
        }
      }
      else if ((uVar14 & 0x100000) == 0) {
        if ((uVar14 & 0x600000) != 0) {
          iVar15 = 0x43;
        }
      }
      else {
        iVar15 = 0x44;
      }
      if ((uVar14 & 0x800) == 0) goto LAB_80176a88;
      uVar16 = uVar14 & 0xc;
      if ((uVar21 & 0x100000) == 0) {
        if (uVar16 != 4) {
          if (uVar16 != 8) goto LAB_80176a88;
          goto LAB_80176a80;
        }
      }
      else {
        if (uVar16 == 4) {
LAB_80176a80:
          iVar15 = 0x58;
          goto LAB_80176a88;
        }
        if (uVar16 != 8) goto LAB_80176a88;
      }
      iVar15 = 0x59;
LAB_80176a88:
      if (((uVar14 & 0x1000) != 0) && (iVar15 = 0x4f, (uVar14 & 3) == 2)) {
        iVar15 = 0x53;
      }
      if ((uVar14 & 0x2000) != 0) {
        if ((uVar14 & 0x800000) == 0) {
          if ((uVar14 & 0xc) == 4) {
            iVar15 = 0x84;
          }
          else if ((uVar14 & 0xc) == 8) {
            iVar15 = 0x85;
          }
          if ((uVar14 & 3) == 1) {
            iVar15 = 0x88;
          }
          else if ((uVar14 & 3) == 2) {
            iVar15 = 0x89;
          }
        }
        else {
          iVar15 = 0x50;
          if ((uVar14 & 3) == 2) {
            iVar15 = 0x51;
          }
        }
      }
      if ((((uVar14 & 0x4000) != 0) && (iVar15 = 0x5b, (uVar14 & 0x800000) == 0)) &&
         (iVar15 = 0x80, (uVar14 & 3) == 2)) {
        iVar15 = 0x82;
      }
      if ((uVar14 & 0x8000) != 0) {
        if ((uVar14 & 0xc) == 4) {
          iVar15 = 0x5f;
        }
        else if ((uVar14 & 0xc) == 8) {
          iVar15 = 0x5e;
        }
        if ((uVar21 & 0x100000) != 0) {
          iVar15 = 0x53;
        }
      }
      if ((uVar14 & 0x10000) != 0) {
        uVar16 = uVar14 & 0xc;
        if ((uVar14 & 0x800000) == 0) {
          if (uVar16 == 4) {
            iVar15 = 0x87;
          }
          else if (uVar16 == 8) {
            iVar15 = 0x86;
          }
          if ((uVar14 & 3) == 1) {
            iVar15 = 0x8c;
          }
          else if ((uVar14 & 3) == 2) {
            iVar15 = 0x8d;
          }
        }
        else {
          if (uVar16 == 4) {
            iVar15 = 0x55;
          }
          else if (uVar16 == 8) {
            iVar15 = 0x56;
          }
          if ((uVar14 & 3) == 1) {
            iVar15 = 0x60;
          }
        }
      }
      if ((uVar14 & 0x40000) == 0) {
        return iVar15;
      }
      if ((uVar14 & 0x800000) == 0) {
        if ((uVar14 & 3) != 2) {
          return 0x81;
        }
        return 0x83;
      }
      if ((uVar14 & 3) != 2) {
        return 0x54;
      }
      return 0x57;
    }
  }
LAB_80176d18:
  uVar11 = *(uint *)(param_1 + 0x6c);
LAB_80176d24:
  *(uint *)(param_1 + 0x6c) = uVar11 & 0xfff7ffff;
  return (int)*(short *)(iVar18 + 0x9a);
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
extern void ev_stream_dispatch(byte *param_1,byte *param_2);
extern void action_switch_guest(int param_1,int *param_2,int param_3);
extern void slot_apply_guest(int param_1,int param_2);
extern void part_record_reset(undefined2 *param_1);
extern ushort * ev5_action_select(int param_1,int param_2,ushort *param_3);
void part_matrix_compute(undefined4 *param_1,byte *param_2,short *param_3,undefined4 param_4);
uint * matrix_vec3_apply(undefined4 *param_1,uint *param_2);
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

void fighter_frame(byte *param_1, uint8_t *param_2)
{

  byte bVar1;
  bool bVar2;
  short sVar3;
  ushort uVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  undefined *puVar9;
  int iVar10;
  undefined4 uVar11;
  int iVar12;
  byte *iVar13;
  
  iVar10 = *(char *)(param_2 + 0x98) * 8;
  iVar12 = iVar10 + -0x7fe4eadc;
  iVar13 = (byte *)guest_at(*(uint32_t *)(param_1 + 0x38));
  bVar2 = false;
  if (param_1[1] == '\0') {
    sVar3 = *(short *)(param_1 + 200);
    iVar5 = (int)sVar3;
    if (iVar5 < 0) {
      if ((DAT_801b55d0 & 1) != 0) {
        (&DAT_801f596c)[(uint)(byte)param_1[2] * 2] = 5;
        (&DAT_801f5970)[(uint)(byte)param_1[2] * 2] = '@' - (char)sVar3;
      }
    }
    else if (iVar5 != 0) {
      iVar5 = iVar5 * 4 + 0x40;
      if (0xff < iVar5) {
        iVar5 = 0xff;
      }
      if ((DAT_801b55d0 & 1) != 0) {
        (&DAT_801f596c)[(uint)(byte)param_1[2] * 2] = 0xf;
        (&DAT_801f5970)[(uint)(byte)param_1[2] * 2] = (char)iVar5;
        (&DAT_801f596d)[(uint)(byte)param_1[2] * 2] = 0xf;
        (&DAT_801f5971)[(uint)(byte)param_1[2] * 2] = 1;
      }
    }
    if ((*(short *)(param_1 + 0xb0) < *(short *)(param_1 + 0xac)) && ((DAT_801b55d0 & 1) != 0)) {
      (&DAT_801f596c)[(uint)(byte)param_1[2] * 2] = 0x78;
      (&DAT_801f5970)[(uint)(byte)param_1[2] * 2] = 0xff;
      (&DAT_801f596d)[(uint)(byte)param_1[2] * 2] = 0x78;
      (&DAT_801f5971)[(uint)(byte)param_1[2] * 2] = 1;
    }
  }
  if (((*(short *)(param_1 + 0xac) < 0) || ((*(uint *)(param_1 + 0x60) & 0x800000) != 0)) ||
     (*param_1 != '\x01')) {
    param_1[200] = '\0';
    param_1[0xc9] = '\0';
  }
  else {
    if ((*(uint *)(param_1 + 0x54) & 2) == 0) {
      sVar3 = *(short *)(param_1 + 0xd8);
      if (sVar3 < 0) {
        if (-1 < *(short *)(param_1 + 0xd6)) {
          *(short *)(param_1 + 0xd6) = *(short *)(param_1 + 0xd6) + *(short *)(param_1 + 0xd8);
          param_1[0xd8] = '\0';
          param_1[0xd9] = '\0';
          if (*(short *)(param_1 + 0xd6) < 0) {
            param_1[0xd6] = -1;
            param_1[0xd7] = -1;
          }
        }
      }
      else if (0 < sVar3) {
        if (*(short *)(param_1 + 0xd6) < 0) {
          param_1[0xd8] = '\0';
          param_1[0xd9] = '\0';
        }
        else {
          *(ushort *)(param_1 + 0xd6) =
               *(short *)(param_1 + 0xd6) + sVar3 * (ushort)*(byte *)((char *)guest_at(*(uint32_t *)(param_2 + 0xc4)) + 3);
          param_1[0xd8] = '\0';
          param_1[0xd9] = '\0';
          if (0x3bff < *(short *)(param_1 + 0xd6)) {
            param_1[0xd6] = -1;
            param_1[0xd7] = ';';
          }
        }
      }
      if ((param_1[5] & 0x10U) == 0) {
        if ((*(short *)(param_1 + 0xd6) < 0) || ((*(uint *)(param_1 + 0x6c) & 0x8000) != 0)) {
          if ((*(uint *)(param_1 + 0x58) & 0x8000) == 0) {
            param_1[0xd6] = '\0';
            param_1[0xd7] = '\0';
          }
        }
        else {
          sVar3 = *(short *)(param_1 + 0xd6) + (ushort)*(byte *)((char *)guest_at(*(uint32_t *)(param_2 + 0xc4)) + 3);
          *(short *)(param_1 + 0xd6) = sVar3;
          if (0x3bff < sVar3) {
            param_1[0xd6] = -1;
            param_1[0xd7] = ';';
          }
        }
      }
      if (*(short *)(param_1 + 0xaa) < *(short *)(param_1 + 0xae)) {
        *(short *)(param_1 + 0xaa) = *(short *)(param_1 + 0xaa) + 1;
      }
    }
    if ((((param_1[5] & 0x10U) != 0) && ((*(uint *)(param_1 + 0x58) & 0x10008000) == 0)) &&
       (*(short *)(param_1 + 0xae) < *(short *)(param_1 + 0xac))) {
      sVar3 = *(short *)(param_1 + 0xfa);
      *(short *)(param_1 + 0xfa) = sVar3 + *(short *)(param_1 + 0xf8);
      iVar5 = (int)(short)(sVar3 + *(short *)(param_1 + 0xf8));
      if (0xff < iVar5) {
        if (iVar5 < 0) {
          iVar5 = iVar5 + 0xff;
        }
        iVar5 = (uint)*(ushort *)(param_1 + 0xac) - (iVar5 >> 8);
        *(short *)(param_1 + 0xac) = (short)iVar5;
        if (iVar5 * 0x10000 >> 0x10 < (int)*(short *)(param_1 + 0xae)) {
          *(undefined2 *)(param_1 + 0xac) = *(undefined2 *)(param_1 + 0xae);
        }
        *(ushort *)(param_1 + 0xfa) = (ushort)(byte)param_1[0xfa];
      }
    }
    sVar3 = *(short *)(param_1 + 200);
    if (*(short *)(param_1 + 200) < 0) {
      if ((*(uint *)(param_1 + 0x54) & 2) == 0) {
        *(short *)(param_1 + 0xac) = *(short *)(param_1 + 0xac) - sVar3;
        *(short *)(param_1 + 0xae) =
             *(short *)(param_1 + 0xae) - (short)((*(short *)(param_1 + 200) * 0x3c) / 100);
      }
      else {
        *(short *)(param_1 + 0xaa) = *(short *)(param_1 + 0xaa) - sVar3;
      }
      if (*(short *)(param_1 + 0xb0) < *(short *)(param_1 + 0xae)) {
        *(undefined2 *)(param_1 + 0xae) = *(undefined2 *)(param_1 + 0xb0);
      }
      if (*(short *)(param_1 + 0xb0) < *(short *)(param_1 + 0xac)) {
        *(undefined2 *)(param_1 + 0xac) = *(undefined2 *)(param_1 + 0xb0);
      }
      sVar3 = *(short *)(param_1 + 200);
      param_1[200] = '\0';
      param_1[0xc9] = '\0';
      sVar3 = -sVar3;
LAB_80178d48:
      *(short *)(param_1 + 0xca) = sVar3;
    }
    else if (*(short *)(param_1 + 200) != 0) {
      if ((*(uint *)(param_1 + 0x54) & 2) == 0) {
        *(short *)(param_1 + 0xac) = *(short *)(param_1 + 0xac) + sVar3;
        *(short *)(param_1 + 0xae) =
             *(short *)(param_1 + 0xae) + (short)((*(short *)(param_1 + 200) * 0x3c) / 100);
      }
      else {
        *(short *)(param_1 + 0xaa) = *(short *)(param_1 + 0xaa) + sVar3;
      }
      if (*(short *)(param_1 + 0xb0) < *(short *)(param_1 + 0xae)) {
        *(undefined2 *)(param_1 + 0xae) = *(undefined2 *)(param_1 + 0xb0);
      }
      sVar3 = *(short *)(param_1 + 200);
      param_1[200] = '\0';
      param_1[0xc9] = '\0';
      goto LAB_80178d48;
    }
    if ((((param_1[5] & 0x10U) != 0) && ((*(uint *)(param_1 + 0x5c) & 0x60000000) == 0)) &&
       ((*(uint *)(param_1 + 0x58) & 0x10008000) == 0)) {
      param_1[0xca] = '\0';
      param_1[0xcb] = '\0';
      param_1[200] = '\0';
      param_1[0xc9] = '\0';
    }
    if (*(short *)(param_1 + 0xb0) < *(short *)(param_1 + 0xac)) {
      bVar2 = true;
      *param_1 = -0x80;
      param_1[0xac] = -1;
      param_1[0xad] = -1;
      param_1[0xae] = -1;
      param_1[0xaf] = -1;
      param_1[0xc0] = -1;
      param_1[0xc1] = -1;
      *(uint *)(param_1 + 0x5c) = *(uint *)(param_1 + 0x5c) | 0x200000;
      param_1[0xe5] = param_1[0xe5] + '\x01';
    }
  }
  motion_turn_update(param_1,iVar13);
  if (((*(uint *)(param_1 + 0x60) & 0x10000000) == 0) &&
     ((*(uint *)(param_1 + 0x6c) & 0x100000) == 0)) {
    sVar3 = range_circle_test(*(undefined4 *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0x174),
                         *(undefined4 *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0x17c),(uint *)guest_at((uint32_t)(iVar10 + -0x7fe4ea74)),0);
    *(short *)(param_1 + 0x94) = sVar3;
    if (sVar3 == 0) {
LAB_80178f9c:
      param_1[0x94] = '\0';
      param_1[0x95] = '\0';
      uVar6 = *(uint *)(param_1 + 0x60) & 0xdfffffff;
    }
    else {
      uVar6 = range_circle_test(*(undefined4 *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0xd4),
                           *(undefined4 *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0xdc),(uint *)guest_at((uint32_t)iVar12),0);
      uVar7 = range_circle_test(*(undefined4 *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0x354),
                           *(undefined4 *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0x35c),(uint *)guest_at((uint32_t)iVar12),0);
      uVar8 = range_circle_test(*(undefined4 *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0x264),
                           *(undefined4 *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0x26c),(uint *)guest_at((uint32_t)iVar12),0);
      if ((uVar6 == 0 && uVar7 == 0) && uVar8 == 0) goto LAB_80178f9c;
      if ((-1 < *(int *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0x38)) &&
         ((*(uint *)(param_1 + 0x6c) & 0x100000) == 0)) {
        if (((uVar6 & uVar7 & uVar8) == 0) && ((*(uint *)(param_1 + 0x58) & 0x1000000) == 0)) {
          if ((*(uint *)(param_1 + 0x5c) & 0x80) != 0) {
            *(uint *)(param_1 + 0x6c) = *(uint *)(param_1 + 0x6c) | 0x80000;
          }
          *(uint *)(param_1 + 0x6c) = *(uint *)(param_1 + 0x6c) | 0x100000;
        }
        if (((*(uint *)(param_1 + 0x5c) & 0x80) == 0) ||
           (0x1ff < *(int *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0x38))) {
          if (*param_1 == '\x01') {
            param_1[0xc0] = -1;
            param_1[0xc1] = -1;
            *param_1 = -0x7f;
          }
          param_1[0xac] = -1;
          param_1[0xad] = -1;
          param_1[0xae] = -1;
          param_1[0xaf] = -1;
          *(uint *)(param_1 + 0x5c) = *(uint *)(param_1 + 0x5c) | 0x80000;
        }
      }
      uVar6 = *(uint *)(param_1 + 0x60) | 0x20000000;
    }
    *(uint *)(param_1 + 0x60) = uVar6;
  }
  if (*(short *)(param_1 + 0x94) == 0) {
    uVar6 = *(uint *)(param_1 + 0x5c) & 0x7fffffff;
  }
  else {
    uVar6 = *(uint *)(param_1 + 0x5c) | 0x80000000;
  }
  *(uint *)(param_1 + 0x5c) = uVar6;
  uVar6 = *(uint *)(param_1 + 0x5c);
  iVar12 = *(int *)(param_1 + 0x20);
  *(uint *)(param_1 + 0x5c) = uVar6 & 0xeffba5fc;
  iVar10 = *(short *)(param_1 + 0x94) + -0x66;
  if (((*(int *)((char *)guest_at((uint32_t)iVar12) + 0x268) < iVar10) && (*(int *)((char *)guest_at((uint32_t)iVar12) + 0x358) < iVar10)) &&
     (*(int *)((char *)guest_at((uint32_t)iVar12) + 0x178) < iVar10)) {
    *(uint *)(param_1 + 0x5c) = uVar6 & 0xeffba5fc | 0x40000;
  }
  if (*(int *)((char *)guest_at(*(uint32_t *)(param_1 + 0x2c)) + 0x58) < 0) {
    *(uint *)(param_1 + 0x5c) = *(uint *)(param_1 + 0x5c) | 0x1000;
  }
  if (((*(uint *)((char *)guest_at(*(uint32_t *)(param_1 + 0x2c)) + 0x58) & 0x30000) != 0) &&
     ((*(uint *)((char *)guest_at(*(uint32_t *)(param_1 + 0x2c)) + 0x58) & 0x40800) == 0)) {
    *(uint *)(param_1 + 0x5c) = *(uint *)(param_1 + 0x5c) | 0xa00;
  }
  if ((*(uint *)((char *)guest_at(*(uint32_t *)(param_1 + 0x2c)) + 0x58) & 0x8040000) == 0x8000000) {
    *(uint *)(param_1 + 0x5c) = *(uint *)(param_1 + 0x5c) | 0x200;
  }
  if (bVar2) {
    if ((param_1[5] & 1U) == 0) {
      bVar1 = param_1[3];
      puVar9 = &DAT_801b4e5c;
    }
    else {
      bVar1 = param_1[3];
      puVar9 = &DAT_801b4e50;
    }
    uVar6 = (uint)(byte)puVar9[bVar1];
    if ((uVar6 < 0xc0) || (uVar11 = 5, param_1[2] == '\0')) {
      uVar11 = 5;
    }
    else {
      uVar6 = uVar6 + 0x40;
    }
LAB_80179238:
    deferred_cmd_post(0x400,uVar6,uVar11);
  }
  else if (((*(uint *)(param_1 + 0x68) & 0x8000000) != 0) &&
          ((*(uint *)(param_1 + 0x5c) & 0x4000000) != 0)) {
    uVar6 = *(uint *)(param_1 + 0x68) & 0x400;
    if ((param_1[5] & 1U) == 0) {
      if (uVar6 == 0) {
        uVar11 = 0x54;
        bVar1 = param_1[3];
        puVar9 = &DAT_801b4e44;
      }
      else {
        uVar11 = 0x55;
        bVar1 = param_1[3];
        puVar9 = &DAT_801b4e38;
      }
      bVar1 = puVar9[bVar1];
      deferred_cmd_post(0x400,uVar11,0);
    }
    else {
      if (uVar6 == 0) {
        uVar11 = 0x54;
      }
      else {
        uVar11 = 0x55;
      }
      deferred_cmd_post(0x400,uVar11,0);
      bVar1 = (&DAT_801b4e2c)[(byte)param_1[3]];
    }
    uVar6 = (uint)bVar1;
    if ((0xbf < uVar6) && (param_1[2] != '\0')) {
      uVar6 = uVar6 + 0x40;
    }
    uVar11 = 0;
    goto LAB_80179238;
  }
  uVar6 = action_resolve(param_1,param_2);
  if (-1 < (int)uVar6) {
    sVar3 = (short)(uVar6 & 0xfff);
    *(short *)(iVar13 + 0x80) = sVar3;
    if (0x8f < (uVar6 & 0xfff)) {
      *(uint *)(param_1 + 0x5c) = *(uint *)(param_1 + 0x5c) & 0xefffbffc;
      *(short *)(iVar13 + 0x14) = sVar3 + -0x90;
    }
    if ((uVar6 & 0x1000) == 0) {
      uVar6 = *(uint *)(param_1 + 0x5c) & 0xffffffbf;
    }
    else {
      uVar6 = *(uint *)(param_1 + 0x5c) | 0x40;
    }
    *(uint *)(param_1 + 0x5c) = uVar6;
  }
  action_commit(param_1,iVar13);
  if ((((*(uint *)(param_1 + 0x6c) & 0x800) != 0) && ((param_1[5] & 0x10U) == 0)) &&
     ((*(uint *)(param_1 + 0x6c) & 0x8000) == 0)) {
    *(ushort *)(param_1 + 0xd8) =
         *(short *)(param_1 + 0xd8) + (ushort)((byte)param_1[0xda] >> 3) * 0x3c;
  }
  dir_rotate_set(((int)*(short *)(param_1 + 0x4e) - (int)*(short *)(iVar13 + 0xd2)) +
               (int)*(short *)(param_1 + 0x8a),(int)*(short *)(param_1 + 0x8c),param_1 + 0xa2,
               param_1 + 0x9a);
  if (*(short *)(param_1 + 0x8c) < 1) {
    if ((-1 < *(short *)(param_1 + 0x8c)) ||
       (uVar4 = *(short *)(param_1 + 0x8c) + *(short *)(param_1 + 0x8e),
       *(ushort *)(param_1 + 0x8c) = uVar4, (int)((uint)uVar4 << 0x10) < 1)) goto LAB_8017938c;
    param_1[0x8e] = '\0';
    param_1[0x8f] = '\0';
  }
  else {
    uVar4 = *(short *)(param_1 + 0x8c) + *(short *)(param_1 + 0x8e);
    *(ushort *)(param_1 + 0x8c) = uVar4;
    if (-1 < (int)((uint)uVar4 << 0x10)) goto LAB_8017938c;
    param_1[0x8e] = '\0';
    param_1[0x8f] = '\0';
  }
  param_1[0x8c] = '\0';
  param_1[0x8d] = '\0';
LAB_8017938c:
  if (*(short *)(param_1 + 0xb2) != 0) {
    dir_rotate_acc((int)*(short *)(param_1 + 0xc4),(int)*(short *)(param_1 + 0xb2),param_1 + 0xa2,
                 param_1 + 0x9a);
    iVar10 = (int)*(short *)(param_1 + 0xb2) * (int)*(short *)(param_1 + 0xb4);
    if (iVar10 < 0) {
      iVar10 = iVar10 + 0x7f;
    }
    *(short *)(param_1 + 0xb2) = (short)(iVar10 >> 7);
  }
  launch_vel_update(param_1,iVar13);
  iVar10 = (int)*(short *)(param_1 + 0x9a) * (int)*(short *)(param_1 + 0x88);
  if (iVar10 < 0) {
    iVar10 = iVar10 + 0xf;
  }
  iVar12 = (int)*(short *)(param_1 + 0x9c) * (int)*(short *)(param_1 + 0x88);
  *(short *)(param_1 + 0x9a) = (short)(iVar10 >> 4);
  if (iVar12 < 0) {
    iVar12 = iVar12 + 0xf;
  }
  iVar10 = (int)*(short *)(param_1 + 0x9e) * (int)*(short *)(param_1 + 0x88);
  *(short *)(param_1 + 0x9c) = (short)(iVar12 >> 4);
  if (iVar10 < 0) {
    iVar10 = iVar10 + 0xf;
  }
  *(short *)(param_1 + 0x9e) = (short)(iVar10 >> 4);
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


void action_commit(byte *param_1, byte *param_2)
{
  bool bVar1;
  ushort uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  undefined1 auStack_48 [32];
  int local_28 [4];
  
  if (*(short *)(param_2 + 0x80) != *(short *)(param_2 + 0x82)) {
    *(undefined2 *)(param_2 + 0x82) = *(undefined2 *)(param_2 + 0x80);
    action_switch_guest((int)guest_of(param_1),(int *)param_2,(int)*(short *)(param_2 + 0x80));
  }
  ev_stream_dispatch(param_1,param_2);
  *(undefined2 *)(param_1 + 0x88) = *(undefined2 *)(param_1 + 0x86);
  if ((((param_1[5] & 1U) != 0) && ((*(uint *)(param_1 + 0x58) & 0xc0000000) == 0x80000000)) &&
     (*(short *)(param_2 + 0x80) != 0xbe)) {
    *(undefined2 *)(param_1 + 0x88) = *(undefined2 *)(param_1 + 0xfc);
  }
  motion_blend_frame(param_1,param_2);
  *(short *)(param_1 + 0xb6) = *(short *)(param_1 + 0xb6) + *(short *)(param_1 + 0x88);
  *(uint *)(param_1 + 0x6c) = *(uint *)(param_1 + 0x6c) & 0xfffff7ff;
  uVar2 = *(ushort *)(param_1 + 0xba);
  if ((0 < *(short *)(param_1 + 0xba)) &&
     (*(short *)(param_1 + 0xba) = (short)((uint)uVar2 - (uint)*(ushort *)(param_1 + 0x88)),
     (int)(((uint)uVar2 - (uint)*(ushort *)(param_1 + 0x88)) * 0x10000) < 1)) {
    param_1[0xba] = '\0';
    param_1[0xbb] = '\0';
    param_1[0x26] = '\0';
    *(uint *)(param_1 + 0x6c) = *(uint *)(param_1 + 0x6c) | 0x800;
  }
  uVar2 = *(ushort *)(param_1 + 0xb8);
  if ((0 < *(short *)(param_1 + 0xb8)) &&
     (*(short *)(param_1 + 0xb8) = (short)((uint)uVar2 - (uint)*(ushort *)(param_1 + 0x86)),
     (int)(((uint)uVar2 - (uint)*(ushort *)(param_1 + 0x86)) * 0x10000) < 1)) {
    param_1[0xb8] = '\0';
    param_1[0xb9] = '\0';
    param_1[0x26] = -1;
  }
  if ((*(int *)(param_1 + 0xb8) == 0) && (-1 < *(short *)(param_1 + 0xbc))) {
    *(short *)(param_1 + 0xbc) = *(short *)(param_1 + 0xbc) + *(short *)(param_1 + 0x88);
  }
  if (0 < *(short *)(param_1 + 0xd0)) {
    uVar2 = *(ushort *)(param_1 + 0xd0);
    param_1[0x27] = -1;
    *(short *)(param_1 + 0xd0) = (short)((uint)uVar2 - (uint)*(ushort *)(param_1 + 0x88));
    if (((int)(((uint)uVar2 - (uint)*(ushort *)(param_1 + 0x88)) * 0x10000) < 1) ||
       (*(short *)(param_2 + 0x80) != 1)) {
      param_1[0xd0] = '\0';
      param_1[0xd1] = '\0';
      param_1[0x27] = '\0';
    }
  }
  if (((*(int *)(param_1 + 100) < 0) && (*(short *)(param_1 + 0xb8) < 0xc0)) &&
     (*(short *)(param_1 + 0xbc) <= *(short *)(param_1 + 0x88))) {
    uVar3 = *(uint *)(param_1 + 0x6c) | 0x4000;
  }
  else {
    uVar3 = *(uint *)(param_1 + 0x6c) & 0xffffbfff;
  }
  *(uint *)(param_1 + 0x6c) = uVar3;
  if (0 < *(short *)(param_1 + 0xd2)) {
    if ((*(ushort *)(param_1 + 0x7a) & 0xf0e8) != 0) {
      *(short *)(param_1 + 0xd2) = *(short *)(param_1 + 0xd2) + *(short *)(param_1 + 0x88) * -2;
    }
    uVar2 = *(ushort *)(param_1 + 0xd2);
    *(short *)(param_1 + 0xd2) = (short)((uint)uVar2 - (uint)*(ushort *)(param_1 + 0x88));
    if ((int)(((uint)uVar2 - (uint)*(ushort *)(param_1 + 0x88)) * 0x10000) < 1) {
      param_1[0xd2] = '\0';
      param_1[0xd3] = '\0';
    }
    pending_hit_a_clear(param_1);
    pending_hit_b_clear(param_1);
  }
  if ((0 < *(short *)(param_2 + 0x116)) &&
     (iVar4 = (uint)*(ushort *)(param_2 + 0x116) - (uint)*(ushort *)(param_1 + 0x88),
     *(short *)(param_2 + 0x116) = (short)iVar4, iVar4 * 0x10000 < 1)) {
    iVar4 = (int)*(short *)(param_2 + 0x114);
    *(undefined2 *)(param_2 + 0x116) = 0;
    if ((-1 < iVar4) && (*(int *)(param_1 + 0x58) < 0)) {
      if ((iVar4 < 0xc0) || (iVar4 = iVar4 + 0x40, param_1[2] == '\0')) {
        iVar4 = (int)*(short *)(param_2 + 0x114);
      }
      deferred_cmd_post(0x400,iVar4,0);
    }
  }
  if ((0 < *(short *)(param_2 + 0x110)) &&
     (iVar4 = (uint)*(ushort *)(param_2 + 0x110) - (uint)*(ushort *)(param_1 + 0x88),
     *(short *)(param_2 + 0x110) = (short)iVar4, iVar4 * 0x10000 < 1)) {
    iVar4 = (int)*(short *)(param_2 + 0x10e);
    *(undefined2 *)(param_2 + 0x110) = 0;
    if (-1 < iVar4) {
      if ((iVar4 < 0xc0) || (iVar4 = iVar4 + 0x40, param_1[2] == '\0')) {
        iVar4 = (int)*(short *)(param_2 + 0x10e);
      }
      deferred_cmd_post(0x400,iVar4,0);
    }
  }
  if (((*(uint *)(param_1 + 0x5c) & 0x10000) == 0) || (*param_1 < '\0')) {
    *(undefined2 *)(param_2 + 0x106) = 0;
    *(undefined2 *)(param_2 + 0x108) = 0;
  }
  else {
    iVar4 = atan2_lut(*(int *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0x3c) -
                         *(int *)((char *)guest_at(*(uint32_t *)((char *)guest_at(*(uint32_t *)(param_1 + 0x2c)) + 0x20)) + 0x3c),
                         *(int *)((char *)guest_at(*(uint32_t *)((char *)guest_at(*(uint32_t *)(param_1 + 0x2c)) + 0x20)) + 0x34) -
                         *(int *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0x34));
    iVar7 = (int)((uint)*(ushort *)((char *)guest_at(*(uint32_t *)(param_1 + 0x40)) + 0x3e) << 0x10) >> 0x11;
    iVar8 = ((iVar4 - *(short *)(param_1 + 0x4e)) + iVar7) * 0x100000 >> 0x14;
    iVar4 = iVar8;
    if (iVar8 < 0) {
      iVar4 = -iVar8;
    }
    if ((0x31c < iVar4) && (bVar1 = -1 < iVar8, iVar8 = -0x31c, bVar1)) {
      iVar8 = 0x31c;
    }
    iVar4 = iVar8;
    if (iVar8 < 0) {
      iVar4 = -iVar8;
    }
    iVar9 = 0;
    if (0x400 < iVar4) {
      iVar5 = -iVar4 + 0x800;
      iVar9 = iVar5 >> 3;
      if (iVar5 < 0) {
        iVar9 = (-iVar4 + 0x807) >> 3;
      }
    }
    iVar8 = iVar8 - ((iVar7 + *(short *)(param_2 + 0x106)) * 0x100000 >> 0x14);
    iVar4 = iVar8;
    if (iVar8 < 0) {
      iVar4 = -iVar8;
    }
    if ((0x2d < iVar4) && (bVar1 = -1 < iVar8, iVar8 = -0x2d, bVar1)) {
      iVar8 = 0x2d;
    }
    *(short *)(param_2 + 0x106) = *(short *)(param_2 + 0x106) + (short)iVar8;
    local_28[1] = 0;
    local_28[0] = *(int *)((char *)guest_at(*(uint32_t *)((char *)guest_at(*(uint32_t *)(param_1 + 0x2c)) + 0x20)) + 0xd4) -
                  *(int *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0xd4);
    local_28[2] = *(int *)((char *)guest_at(*(uint32_t *)((char *)guest_at(*(uint32_t *)(param_1 + 0x2c)) + 0x20)) + 0xdc) -
                  *(int *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0xdc);
    gte_op_a00428(local_28,local_28);
    iVar8 = *(int *)((char *)guest_at(*(uint32_t *)((char *)guest_at(*(uint32_t *)(param_1 + 0x2c)) + 0x20)) + 0xd8);
    iVar4 = *(int *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0xd8);
    uVar6 = FUN_80194500(local_28[0] + local_28[2]);
    iVar4 = atan2_lut(uVar6,iVar8 - iVar4);
    iVar8 = atan2_lut((int)*(short *)((char *)guest_at(*(uint32_t *)(param_1 + 0x40)) + 0x3a),
                         (int)*(short *)((char *)guest_at(*(uint32_t *)(param_1 + 0x40)) + 0x3c));
    iVar4 = (iVar4 + iVar8 + iVar9) * 0x100000 >> 0x14;
    if (iVar4 < -0x271) {
      iVar4 = -0x271;
    }
    else if (0x1c7 < iVar4) {
      iVar4 = 0x1c7;
    }
    iVar4 = iVar4 - ((iVar8 + *(short *)(param_2 + 0x108)) * 0x100000 >> 0x14);
    iVar8 = iVar4;
    if (iVar4 < 0) {
      iVar8 = -iVar4;
    }
    if ((0x22 < iVar8) && (bVar1 = -1 < iVar4, iVar4 = -0x22, bVar1)) {
      iVar4 = 0x22;
    }
    iVar4 = (uint)*(ushort *)(param_2 + 0x108) + iVar4;
    *(short *)(param_2 + 0x108) = (short)iVar4;
    euler_rotmat(iVar4 * 0x10000 >> 0x10,-(int)*(short *)(param_2 + 0x106),0,auStack_48);
    matrix_vec3_apply(auStack_48,(uint *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0xa4));
  }
  return;

}

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

void trig_lut_read(uint32_t ctrl_g, byte *mo)
{
  int angle;
  int vel;
  
  angle = (((int)((uint)*(byte *)guest_at(ctrl_g + 0xdb) * 0x60) >> 3) + 0x200) *
          (0x41 - (uint)*(byte *)guest_at(ctrl_g + 0xdd));
  if (angle < 0) {
    angle = angle + 0x3f;
  }
  *(undefined2 *)(mo + 0xb4) = 0x78;
  *(undefined2 *)(mo + 0xb2) = 0;
  *(undefined2 *)(mo + 0x92) = 0xa0;
  vel = lut_cos_q12(angle >> 6);
  vel = vel * 0xd00;
  if (vel < 0) {
    vel = vel + 0xfff;
  }
  *(short *)(mo + 0x90) = (short)(vel >> 0xc);
  angle = lut_sin_q12(angle >> 6);
  angle = angle * -0x500;
  if (angle < 0) {
    angle = angle + 0xfff;
  }
  *(short *)(mo + 0x98) = (short)(angle >> 0xc);
  if (((*(byte *)guest_at(ctrl_g + 5) & 0x10) != 0) && ((*(uint *)guest_at(ctrl_g + 100) & 0x480000) != 0)) {
    angle = *(short *)(mo + 0x90) * 0x140;
    if (angle < 0) {
      angle = angle + 0xff;
    }
    *(short *)(mo + 0x90) = (short)((uint)angle >> 8);
  }
  *(short *)(mo + 0x90) =
       (short)mips_div_lo(*(short *)(mo + 0x90) * 0x12c0, *(short *)(mo + 0x24) + 4000);
  *(short *)(mo + 0x98) =
       (short)mips_div_lo(*(short *)(mo + 0x98) * 0x12c0, *(short *)(mo + 0x24) + 4000);
  return;

}

void trig_vel_flat(uint32_t ctrl_g, byte *mo)
{
  short vel;
  int angle;
  
  angle = ((uint)*(byte *)guest_at(ctrl_g + 0xdb) * 8 + 0x400) * (0x41 - (uint)*(byte *)guest_at(ctrl_g + 0xdd));
  if (angle < 0) {
    angle = angle + 0x3f;
  }
  *(undefined2 *)(mo + 0xb4) = 0x78;
  *(undefined2 *)(mo + 0xb2) = 0;
  *(undefined2 *)(mo + 0x92) = 0xa0;
  angle = lut_cos_q12(angle >> 6);
  vel = (short)(angle / 2);
  *(short *)(mo + 0x90) = vel;
  *(undefined2 *)(mo + 0x98) = 0;
  *(short *)(mo + 0x90) = (short)mips_div_lo(vel * 0x12c0, *(short *)(mo + 0x24) + 4000);
  return;

}

void trig_vel_up(uint32_t ctrl_g, byte *mo)
{
  undefined2 cos_v;
  short sin_v;
  int angle;
  
  angle = ((int)((uint)*(byte *)guest_at(ctrl_g + 0xdb) * 0x10) >> 2) + 0x300;
  *(undefined2 *)(mo + 0xb4) = 0x78;
  *(undefined2 *)(mo + 0xb2) = 0;
  *(undefined2 *)(mo + 0x92) = 0xa0;
  cos_v = lut_cos_q12(angle);
  *(undefined2 *)(mo + 0x90) = cos_v;
  sin_v = lut_sin_q12(angle);
  *(short *)(mo + 0x98) = sin_v * -2;
  if ((*(byte *)guest_at(ctrl_g + 5) & 0x10) != 0) {
    *(short *)(mo + 0x98) = sin_v * -2 + -0x2000;
  }
  *(short *)(mo + 0x90) =
       (short)mips_div_lo(*(short *)(mo + 0x90) * 0x12c0, *(short *)(mo + 0x24) + 4000);
  *(short *)(mo + 0x98) =
       (short)mips_div_lo(*(short *)(mo + 0x98) * 0x12c0, *(short *)(mo + 0x24) + 4000);
  return;

}

void trig_vel_diag(uint32_t ctrl_g, byte *mo)
{
  undefined2 cos_v;
  short sin_v;
  int angle;
  
  angle = (int)((uint)*(byte *)guest_at(ctrl_g + 0xdb) * 0x10) >> 2;
  *(undefined2 *)(mo + 0xb4) = 0x78;
  *(undefined2 *)(mo + 0xb2) = 0;
  *(undefined2 *)(mo + 0x92) = 0xa0;
  cos_v = lut_cos_q12(angle);
  *(undefined2 *)(mo + 0x90) = cos_v;
  sin_v = lut_sin_q12(angle);
  *(short *)(mo + 0x98) = -sin_v;
  *(short *)(mo + 0x90) =
       (short)mips_div_lo(*(short *)(mo + 0x90) * 0x12c0, *(short *)(mo + 0x24) + 4000);
  *(short *)(mo + 0x98) =
       (short)mips_div_lo(*(short *)(mo + 0x98) * 0x12c0, *(short *)(mo + 0x24) + 4000);
  return;

}
void launch_vel_update(byte *mo,byte *aux)
{
  ushort acc;
  int state;
  int vx;
  int vy;
  int rem;
  vx = (int)*(short *)(mo + 0x90);
  vy = (int)*(short *)(mo + 0x92);
  state = *(int *)((char *)guest_at(*(uint32_t *)(mo + 0x20)) + 0x38);
  rem = 0;
  if ((*(int *)(mo + 0x5c) < 0) && (state < *(short *)(mo + 0x94))) {
    vy = 0xa0;
    *(undefined2 *)(mo + 0x92) = 0xa0;
  }
  if ((state == *(short *)(mo + 0x94)) && (vx == 0 && vy == 0)) {
    *(undefined2 *)(mo + 0x9c) = 0;
    *(undefined2 *)(mo + 0xa4) = 0;
    return;
  }
  if (((*(uint *)(mo + 0x60) & 0x2000) == 0) && (vx != 0 || vy != 0)) {
    dir_rotate_acc((int)*(short *)(aux + 0xd0) + (int)*(short *)(aux + 0xe4) +
                 (int)*(short *)(mo + 0x96),(int)*(short *)(mo + 0x98),mo + 0xa2,
                 mo + 0x9a);
    if (*(short *)(mo + 0x98) < 1) {
      if ((*(short *)(mo + 0x98) < 0) &&
         (acc = *(short *)(mo + 0x98) + *(short *)(mo + 0x8e),
         *(ushort *)(mo + 0x98) = acc, 0 < (int)((uint)acc << 0x10))) {
        *(undefined2 *)(mo + 0x8e) = 0;
        goto LAB_8017957c;
      }
    }
    else {
      acc = *(short *)(mo + 0x98) + *(short *)(mo + 0x8e);
      *(ushort *)(mo + 0x98) = acc;
      if ((int)((uint)acc << 0x10) < 0) {
        *(undefined2 *)(mo + 0x8e) = 0;
LAB_8017957c:
        *(undefined2 *)(mo + 0x98) = 0;
      }
    }
  }
  if (((*(uint *)(mo + 0x60) & 0x800) == 0) && (vx < 1)) {
    vy = vy * *(short *)(mo + 0x88);
    if (vy < 0) {
      vy = vy + 0xf;
    }
    vy = vx + (vy >> 4);
  }
  else {
    if (((*(uint *)(mo + 0x60) & 0x1000) != 0) || (vx < 1)) goto LAB_80179618;
    vy = vy * *(short *)(mo + 0x88);
    if (vy < 0) {
      vy = vy + 0xf;
    }
    vy = vx + (vy >> 4);
    if (vy < -0x8000) {
      vy = -0x8000;
    }
  }
  *(short *)(mo + 0x90) = (short)vy;
  rem = (short)(vx >> 7);
LAB_80179618:
  *(undefined2 *)(mo + 0x9c) = rem;
  return;

}

void motion_turn_update(byte *mo,byte *ctrl)
{
  ushort ctr16;
  uint flags;
  int abs_rate;
  int rate;
  int step;
  rate = (int)*(short *)(mo + 0x74);
  abs_rate = rate;
  if (rate < 0) {
    abs_rate = -rate;
  }
  step = 0;
  if (abs_rate < 0x401) {
    flags = *(uint *)(mo + 0x5c) & 0xffbfffff;
  }
  else {
    flags = *(uint *)(mo + 0x5c) | 0x400000;
  }
  *(uint *)(mo + 0x5c) = flags;
  rate = (rate + *(short *)(ctrl + 0xd2)) * 0x100000 >> 0x14;
  abs_rate = rate;
  if (rate < 0) {
    abs_rate = -rate;
  }
  if (abs_rate < 0x2ab) {
    flags = *(uint *)(mo + 0x5c) & 0xfffffbff;
  }
  else {
    flags = *(uint *)(mo + 0x5c) | 0x400;
  }
  *(uint *)(mo + 0x5c) = flags;
  abs_rate = rate;
  if (rate < 0) {
    abs_rate = -rate;
  }
  if (abs_rate < 0x556) {
    flags = *(uint *)(mo + 0x5c) & 0xffefffff;
  }
  else {
    flags = *(uint *)(mo + 0x5c) | 0x100000;
  }
  *(uint *)(mo + 0x5c) = flags;
  if ((*(uint *)((char *)guest_at(*(uint32_t *)(mo + 0x2c)) + 0x60) & 0x4000000) == 0) {
    if ((*(uint *)(mo + 0x5c) & 0x20000) == 0) {
      if ((*(uint *)(mo + 0x60) & 0x2000000) != 0) {
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
      if ((*(uint *)(mo + 0x5c) & 0x8000000) == 0) {
        abs_rate = rate;
        if (rate < 0) {
          abs_rate = -rate;
        }
        if ((*(short *)(ctrl + 0x10c) <= abs_rate) && ((*(ushort *)(ctrl + 0x10a) & 0x80) == 0))
        goto LAB_801798e0;
      }
      else {
        abs_rate = rate;
        if (rate < 0) {
          abs_rate = -rate;
        }
        if ((abs_rate <= *(short *)(ctrl + 0x10c)) && ((*(ushort *)(ctrl + 0x10a) & 0x80) == 0))
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
  ctr16 = *(ushort *)(ctrl + 0x10a) - 1;
  if (((*(ushort *)(ctrl + 0x10a) & 0x7f) != 0) &&
     (*(ushort *)(ctrl + 0x10a) = ctr16, (ctr16 & 0x7f) == 0)) {
    *(uint *)(mo + 0x5c) = *(uint *)(mo + 0x5c) & 0xfffdffff;
  }
  if (step != 0) {
    *(short *)(mo + 0x74) = (short)((*(short *)(mo + 0x74) - step) * 0x100000 >> 0x14);
    *(short *)(mo + 0x4e) = (short)((*(short *)(mo + 0x4e) + step) * 0x100000 >> 0x14);
    *(short *)(ctrl + 0xe4) = (short)((*(short *)(ctrl + 0xe4) + step) * 0x100000 >> 0x14);
  }
  return;

}

void motion_blend_frame(byte *param_1,byte *param_2)
{
  short sVar1;
  undefined2 uVar2;
  short sVar3;
  int iVar4;
  int iVar5;
  undefined2 *puVar6;
  undefined2 *puVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  if (*(int *)(param_2 + 0xb0) == 0) goto LAB_8018604c;
  if (*(short *)(param_2 + 0xce) == 0) {
LAB_80186010:
    keyframe_blend_apply(param_1,param_2);
  }
  else {
    if (*(short *)(param_2 + 0xca) < 1) {
      *(short *)(param_2 + 0xc6) = *(short *)(param_2 + 0xc6) + 1;
      keyframe_advance(param_1,param_2);
    }
    if (*(short *)(param_2 + 0xce) == 0) {
      if (*(short *)(param_2 + 0xc0) == 0) goto LAB_80186010;
    }
    else {
      keyframe_blend_apply(param_1,param_2);
      *(short *)(param_2 + 0xca) = *(short *)(param_2 + 0xca) - *(short *)(param_2 + 0xce);
    }
  }
  if (*(short *)(param_2 + 0xc4) < *(short *)(param_2 + 0xc2)) {
    *(short *)(param_2 + 0xc4) = *(short *)(param_2 + 0xc4) + *(short *)(param_1 + 0x88);
  }
LAB_8018604c:
  iVar9 = 0;
  puVar6 = (undefined2 *)((char *)guest_at(*(uint32_t *)(param_1 + 0x40)) + 0x12);
  puVar7 = (undefined2 *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0x3c);
  do {
    puVar6[-2] = puVar7[-4];
    iVar9 = iVar9 + 1;
    puVar6[-1] = puVar7[-2];
    uVar2 = *puVar7;
    puVar7 = puVar7 + 0x28;
    *puVar6 = uVar2;
    puVar6 = puVar6 + 0xe;
  } while (iVar9 < 0x11);
  skeleton_pose_step((byte *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0x50,
               (undefined2 *)((char *)guest_at(*(uint32_t *)(param_1 + 0x40)) + 0x1c),
               (byte *)((char *)guest_at(*(uint32_t *)(param_1 + 0x48)) + 0x20),0x12);
  iVar11 = *(int *)(param_1 + 0x20);
  iVar9 = -(int)*(short *)(param_2 + 0xe4);
  if (iVar9 != 0) {
    iVar10 = (int)*(short *)(param_2 + 0xd4) - *(int *)((char *)guest_at((uint32_t)iVar11) + 0x34);
    iVar8 = (int)*(short *)(param_2 + 0xd8) - *(int *)((char *)guest_at((uint32_t)iVar11) + 0x3c);
    iVar4 = lut_sin_q12(iVar9);
    iVar5 = lut_cos_q12(iVar9);
    *(short *)(param_2 + 0xd4) =
         *(short *)((char *)guest_at((uint32_t)iVar11) + 0x34) + (short)((iVar8 * iVar4 + iVar10 * iVar5) >> 0xc);
    iVar4 = lut_cos_q12(iVar9);
    iVar9 = lut_sin_q12(iVar9);
    sVar3 = *(short *)((char *)guest_at((uint32_t)iVar11) + 0x3c);
    sVar1 = *(short *)(param_2 + 0xe4);
    *(undefined2 *)(param_2 + 0xe4) = 0;
    *(short *)(param_2 + 0xd0) = (short)((*(short *)(param_2 + 0xd0) + sVar1) * 0x10) >> 4;
    *(short *)(param_2 + 0xd8) = sVar3 + (short)((iVar8 * iVar4 - iVar10 * iVar9) >> 0xc);
  }
  iVar9 = ((int)*(short *)(param_2 + 0xd0) + (int)*(short *)(param_2 + 0xd2) +
          (int)*(short *)(param_2 + 0xe4)) * 0x100000;
  *(short *)(param_1 + 0x4e) = (short)(iVar9 >> 0x14);
  euler_rotmat((int)*(short *)(param_1 + 0x4c),-(iVar9 >> 0x14),(int)*(short *)(param_1 + 0x50),
               (undefined2 *)((char *)guest_at((uint32_t)iVar11) + 4));
  iVar9 = *(int *)(param_1 + 0x20);
  if ((*(uint *)(param_1 + 0x54) & 8) == 0) {
    iVar11 = *(int *)(param_1 + 0x2c);
    if ((*(uint *)((char *)guest_at((uint32_t)iVar11) + 0x54) & 8) == 0) {
      if ((*(uint *)(param_1 + 0x54) & 0x10) == 0) {
        if ((*(uint *)((char *)guest_at((uint32_t)iVar11) + 0x54) & 0x10) == 0) {
          return;
        }
        if (((*(uint *)(param_1 + 0x58) & 0x40100000) == 0) &&
           ((*(uint *)((char *)guest_at((uint32_t)iVar11) + 0x58) & 0x40100000) == 0)) {
          return;
        }
      }
      iVar11 = *(short *)((char *)guest_at((uint32_t)iVar9) + 0x6a) * 7;
      if (iVar11 < 0) {
        iVar11 = iVar11 + 7;
      }
      *(short *)((char *)guest_at((uint32_t)iVar9) + 0x6a) = (short)(iVar11 >> 3);
      return;
    }
    if (((*(uint *)(param_1 + 0x58) & 0x40100000) == 0) &&
       ((*(uint *)((char *)guest_at((uint32_t)iVar11) + 0x58) & 0x40100000) == 0)) {
      return;
    }
  }
  iVar11 = (uint)*(ushort *)((char *)guest_at((uint32_t)iVar9) + 0x6a) << 0x10;
  *(short *)((char *)guest_at((uint32_t)iVar9) + 0x6a) = (short)(((iVar11 >> 0x10) - (iVar11 >> 0x1f)) >> 1);
  return;

}

void deferred_cmd_post(uint param_1,int param_2,short param_3)
{
  int iVar1;
  ushort *puVar2;
  short *psVar3;
  if (((param_1 & 0xff) != 0) || (-1 < param_2)) {
    if (param_1 == 0) {
      deferred_cmd_reset();
    }
    else if ((param_1 & 0x1000) == 0) {
      iVar1 = 0;
      if (_DAT_801f35f4 < 0x20) {
        psVar3 = (short *)&DAT_801f3600;
        do {
          iVar1 = iVar1 + 1;
          if (*psVar3 == 0) break;
          psVar3 = psVar3 + 3;
        } while (iVar1 < 0x20);
        *psVar3 = (short)param_1;
        psVar3[1] = (short)param_2;
        psVar3[2] = param_3;
        _DAT_801f35f4 = _DAT_801f35f4 + 1;
      }
    }
    else {
      puVar2 = (ushort *)&DAT_801f3600;
      iVar1 = 0;
      do {
        if (((*puVar2 & 0xfff) == (param_1 & 0xfff)) && ((short)puVar2[1] == param_2)) {
          *puVar2 = 0;
          _DAT_801f35f4 = _DAT_801f35f4 + -1;
        }
        iVar1 = iVar1 + 1;
        puVar2 = puVar2 + 3;
      } while (iVar1 < 0x20);
    }
  }
  return;

}

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

int atan2_lut(int param_1,int param_2)
{
  bool bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  iVar4 = 0;
  iVar2 = 0;
  if (param_1 != 0 || param_2 != 0) {
    iVar2 = 0x800;
    bVar1 = param_2 < 1;
    if (bVar1) {
      param_2 = -param_2;
      iVar2 = -0x800;
    }
    if (param_1 < 0) {
      param_1 = -param_1;
      bVar1 = !bVar1;
      iVar4 = iVar2;
    }
    iVar3 = param_1;
    if (param_1 < param_2) {
      bVar1 = (bool)(bVar1 ^ 1);
      iVar4 = iVar2 >> 1;
      iVar3 = param_2;
      param_2 = param_1;
    }
    iVar2 = (int)*(short *)(&DAT_80123844 + ((param_2 << 0xc) / iVar3 >> 2) * 2);
    if (bVar1) {
      iVar2 = -iVar2;
    }
  }
  return iVar4 + iVar2;

}


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


void slot_mark2_set(int slot)
{
  (&DAT_801f0ad8)[slot] = 2;
  return;

}

void fighter_reset_fx(int reset,byte *ctrl)
{
  if ((DAT_1f8000b1 == '\0') && (reset == 1)) {
    fx_record_alloc(*(int *)(ctrl + 0x20) + 0x124,6,0x8000000e,0x83,&DAT_801b1d48);
    quad_fx_spawn((int)guest_of(ctrl),0x12,(undefined4 *)guest_at((uint32_t)(*(int *)(ctrl + 0x20) + 0x124)),0x420,0x3000,2,2);
    triple_track_spawn(*(undefined1 *)(ctrl + 2),0x30,0);
  }
  return;

}
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

unsigned long long dir_rotate_wrap(undefined4 param_1,int param_2,short *param_3)
{
  ushort uVar1;
  ushort uVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  undefined8 uVar7;
  
  if (param_2 == 0) {
    return 0;
  }
  iVar5 = (int)*param_3;
  iVar6 = (int)param_3[2];
  uVar7 = vec2_rot_lut(param_1,param_2);
  iVar3 = (int)((ulonglong)uVar7 >> 0x20) + iVar5 + 0x40;
  iVar5 = (int)uVar7 + iVar6 + 0x40;
  uVar1 = (ushort)iVar3;
  uVar2 = (ushort)iVar5;
  uVar4 = iVar3 >> 7;
  *param_3 = (uVar1 - (uVar1 & 0xff80)) + -0x40;
  param_3[2] = (uVar2 - (uVar2 & 0xff80)) + -0x40;
  return CONCAT44(uVar4,(iVar5 >> 7) << 0x10 | uVar4 & 0xffff) & 0xffffffffffff;

}

void keyframe_blend_apply(byte *param_1,byte *param_2)
{
  extern void decomp_trace_record(uint8_t,uint32_t,uint32_t,uint8_t,uint32_t);
  byte bVar1;
  short sVar2;
  byte *pbVar3;
  int iVar4;
  int iVar5;
  short *psVar6;
  short *psVar7;
  int *piVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  uint uVar13;
  int iVar14;
  
  uVar13 = (uint)*(byte *)(param_2 + 0x100);
  iVar14 = *(int *)(param_2 + uVar13 * 4 + 0xf8);
  if ((int)*(short *)(param_2 + 0xce) < (int)*(short *)(param_2 + 0xca)) {
    /* guest div @0x801859F8: ca==0 -> LO = (dividend<0)?1:-1, no trap */
    iVar12 = mips_div_lo((int)*(short *)(param_2 + 0xce) << 0xc,
                         (int)*(short *)(param_2 + 0xca));
  }
  else {
    iVar12 = 0x1000;
  }
  anim_lerp_scalar((short *)(param_2 + 0xd2),(int)*(short *)guest_at(*(uint32_t *)(param_2 + 0xb8)),uVar13,iVar12);
  anim_lerp_vec3((short *)(param_2 + 0xdc),(short *)((char *)guest_at(*(uint32_t *)(param_2 + 0xb8)) + 2),uVar13,iVar12,0x80);
  anim_lerp_vec3((short *)((char *)guest_at(*(uint32_t *)(param_1 + 0x40)) + 0x24),(short *)((char *)guest_at(*(uint32_t *)(param_2 + 0xb8)) + 8),uVar13,iVar12,
               (int)*(short *)(param_2 + 0xbe));
  anim_lerp_vec3_raw((short *)((char *)guest_at(*(uint32_t *)(param_1 + 0x40)) + 0x78),(short *)((char *)guest_at(*(uint32_t *)(param_2 + 0xb8)) + 0xe),uVar13,iVar12);
  iVar5 = *(int *)(param_1 + 0x40);
  iVar4 = *(int *)(param_2 + 0xb8);
  anim_lerp_packed4((short *)guest_at((uint32_t)(iVar5 + 0x1c)),(ushort *)guest_at((uint32_t)(iVar4 + 0x14 + (uint)*(byte *)((char *)guest_at((uint32_t)iVar14) + 1) * 6)),0,iVar12);
  iVar5 = iVar5 + 0x38;
  iVar11 = 2;
  do {
    iVar10 = iVar11;
    iVar9 = iVar5;
    iVar11 = iVar10 + 1;
    anim_lerp_packed4((short *)guest_at((uint32_t)iVar9),(ushort *)guest_at((uint32_t)(iVar4 + 0x14 + (uint)*(byte *)((char *)guest_at((uint32_t)iVar14) + iVar10) * 6)),uVar13,iVar12);
    iVar5 = iVar9 + 0x1c;
  } while (iVar11 < 0x11);
  anim_lerp_aux((byte *)guest_at((uint32_t)(iVar9 + 0x1c)),
               (int)*(short *)guest_at((uint32_t)((uint)*(byte *)((char *)guest_at((uint32_t)iVar14) + iVar11) * 2 + iVar4 + 0x74 + -0x20)),uVar13,
               iVar12);
  anim_lerp_aux((byte *)guest_at((uint32_t)(iVar9 + 0x38)),
               (int)*(short *)guest_at((uint32_t)((uint)*(byte *)((char *)guest_at((uint32_t)iVar14) + iVar10 + 2) * 2 + iVar4 + 0x74 + -0x20)),
               uVar13,iVar12);
  iVar5 = *(int *)(param_1 + 0x40);
  bVar1 = *(byte *)((char *)guest_at((uint32_t)iVar4) + 0x78);
  pbVar3 = (byte *)guest_at((uint32_t)iVar4) + 0x78;
  while (bVar1 != 0xff) {
    {
      uint32_t _idx = (uint)*(byte *)((char *)guest_at((uint32_t)iVar14) + (uint)bVar1);
      uint32_t _dst = (uint32_t)iVar5 + _idx * 0x1c + 0x24;
      if (_dst >= 0x801d0000u && _dst < 0x80200000u)
        decomp_trace_record(52, _dst, (uint32_t)iVar5, (uint8_t)_idx, 0);
    }
    anim_lerp_packed3((short *)guest_at((uint32_t)(iVar5 + (uint)*(byte *)((char *)guest_at((uint32_t)iVar14) + (uint)bVar1) * 0x1c + 0x24)),(ushort *)pbVar3,uVar13,iVar12);
    bVar1 = pbVar3[4];
    pbVar3 = pbVar3 + 4;
  }
  iVar14 = 1;
  psVar7 = (short *)((char *)guest_at(*(uint32_t *)(param_1 + 0x20)) + 0x6c);
  psVar6 = (short *)((char *)guest_at(*(uint32_t *)(param_1 + 0x40)) + 0x28);
  piVar8 = (int *)guest_at(*(uint32_t *)((char *)guest_at(*(uint32_t *)(param_2 + 0x10)) + (*(byte *)(param_1 + 5) & 1) * 0x1c + 8) +
                  *(uint32_t *)(param_1 + 0x30));
  do {
    iVar14 = iVar14 + 1;
    iVar12 = *piVar8;
    piVar8 = piVar8 + 1;
    psVar7[-2] = (short)((iVar12 << 0x16) >> 0x16) + (psVar6[-2] >> 4);
    psVar7[-1] = (short)((iVar12 << 0xc) >> 0x16) + (psVar6[-1] >> 4);
    sVar2 = *psVar6;
    psVar6 = psVar6 + 0xe;
    *psVar7 = (short)((iVar12 << 2) >> 0x16) + (sVar2 >> 4);
    psVar7 = psVar7 + 0x28;
  } while (iVar14 < 0x13);
  return;

}

void keyframe_advance(byte *param_1,byte *param_2)
{
  short sVar1;
  undefined4 uVar2;
  undefined2 local_8;
  undefined2 uStack_6;
  
  uVar2 = *(undefined4 *)((char *)guest_at(*(uint32_t *)(param_2 + 0xb4)) + *(short *)(param_2 + 0xc6) * 4);
  sVar1 = *(short *)(param_2 + 0xcc);
  *(short *)(param_2 + 200) = sVar1;
  if (sVar1 == 0) {
    *(undefined2 *)(param_2 + 0xce) = 0;
    *(undefined2 *)(param_2 + 0xca) = 0;
  }
  else {
    *(short *)(param_2 + 0xca) = *(short *)(param_2 + 0xca) + sVar1;
    *(undefined2 *)(param_2 + 0xce) = *(undefined2 *)(param_1 + 0x88);
    local_8 = (short)uVar2;
    *(int *)(param_2 + 0xb8) = (int)local_8 + *(int *)(param_2 + 0xb0);
    uStack_6 = (short)((uint)uVar2 >> 0x10);
    *(short *)(param_2 + 0xcc) = uStack_6 << 4;
  }
  return;

}

void skeleton_pose_step(byte *param_1,undefined2 *param_2,byte *param_3,int param_4)
{
  ushort uVar1;
  ushort *puVar2;
  byte *pbVar3;
  int iVar4;
  
  iVar4 = 0;
  if (0 < param_4 + -2) {
    puVar2 = param_2 + 3;
    pbVar3 = param_1 + 4;
    do {
      puVar2[4] = *(ushort *)(pbVar3 + 0x30);
      puVar2[5] = *(ushort *)(pbVar3 + 0x34);
      puVar2[6] = *(ushort *)(pbVar3 + 0x38);
      _DAT_1f80038c = *param_2;
      iVar4 = iVar4 + 1;
      _DAT_1f80038e = puVar2[-2];
      _DAT_1f800390 = puVar2[-1];
      uVar1 = *puVar2;
      puVar2 = puVar2 + 0xe;
      part_matrix_compute((undefined4 *)pbVar3,param_3,&DAT_1f80038c,(int)((uint)uVar1 << 0x10) >> 0x11);
      param_1 = param_1 + 0x50;
      param_2 = param_2 + 0xe;
      param_3 = param_3 + 0x20;
      pbVar3 = pbVar3 + 0x50;
    } while (iVar4 < param_4 + -2);
  }
  rotmat_rtps_build(-(int)(short)param_2[3] >> 1,(uint *)(param_1 + 4));
  rotmat_rtps_build(-(int)(short)param_2[0x11] >> 1,(uint *)(param_1 + 0x54));
  return;

}

void deferred_cmd_reset(void)
{
  undefined2 *puVar1;
  int iVar2;
  
  _DAT_801f35f4 = 0;
  iVar2 = 0x1f;
  puVar1 = (undefined2 *)&DAT_801f36ba;
  do {
    *puVar1 = 0;
    iVar2 = iVar2 + -1;
    puVar1 = puVar1 + -3;
  } while (-1 < iVar2);
  return;

}
void anim_lerp_scalar(short *param_1,short param_2,int param_3,int param_4)
{
  int iVar1;
  
  iVar1 = (int)param_2;
  if (param_3 != 0) {
    iVar1 = -iVar1;
  }
  param_4 = (iVar1 - *param_1) * param_4;
  if (param_4 < 0) {
    param_4 = param_4 + 0xfff;
  }
  *param_1 = *param_1 + (short)(param_4 >> 0xc);
  return;

}

void anim_lerp_vec3(short *param_1,short *param_2,int param_3,int param_4,short param_5)
{
  short sVar1;
  int iVar2;
  int iVar3;
  
  if (param_3 == 0) {
    iVar2 = (int)*param_2;
  }
  else {
    iVar2 = -(int)*param_2;
  }
  iVar3 = (int)param_2[1];
  if (0 < iVar3) {
    iVar3 = iVar3 * param_5 >> 7;
  }
  iVar2 = (iVar2 - *param_1) * param_4;
  sVar1 = param_2[2];
  if (iVar2 < 0) {
    iVar2 = iVar2 + 0xfff;
  }
  iVar3 = (iVar3 - param_1[1]) * param_4;
  *param_1 = *param_1 + (short)(iVar2 >> 0xc);
  if (iVar3 < 0) {
    iVar3 = iVar3 + 0xfff;
  }
  param_4 = ((int)sVar1 - (int)param_1[2]) * param_4;
  param_1[1] = param_1[1] + (short)(iVar3 >> 0xc);
  if (param_4 < 0) {
    param_4 = param_4 + 0xfff;
  }
  param_1[2] = param_1[2] + (short)(param_4 >> 0xc);
  return;

}

void anim_lerp_vec3_raw(short *param_1,short *param_2,int param_3,int param_4)
{
  short sVar1;
  int iVar2;
  int iVar3;
  
  if (param_3 == 0) {
    iVar3 = (int)param_2[1];
  }
  else {
    iVar3 = -(int)param_2[1];
  }
  iVar2 = ((int)*param_2 - (int)*param_1) * param_4;
  sVar1 = param_2[2];
  if (iVar2 < 0) {
    iVar2 = iVar2 + 0xfff;
  }
  iVar3 = (iVar3 - param_1[1]) * param_4;
  *param_1 = *param_1 + (short)(iVar2 >> 0xc);
  if (iVar3 < 0) {
    iVar3 = iVar3 + 0xfff;
  }
  param_4 = ((int)sVar1 - (int)param_1[2]) * param_4;
  param_1[1] = param_1[1] + (short)(iVar3 >> 0xc);
  if (param_4 < 0) {
    param_4 = param_4 + 0xfff;
  }
  param_1[2] = param_1[2] + (short)(param_4 >> 0xc);
  return;

}

void anim_lerp_packed4(short *param_1,ushort *param_2,int param_3,int param_4)
{
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  {
    extern void decomp_trace_record(uint8_t,uint32_t,uint32_t,uint8_t,uint32_t);
    uint32_t _gd = (uint32_t)guest_of(param_1);
    if (_gd >= 0x801d0000u && _gd < 0x80200000u)
      decomp_trace_record(53, _gd, (uint32_t)guest_of(param_2), 4, 0);
  }
  
  uVar1 = (uint)*param_2;
  iVar5 = uVar1 + (uint)param_2[1] * 0x10000;
  if (param_3 == 0) {
    iVar2 = (int)(short)param_2[2];
    iVar3 = (int)(uVar1 << 0x16) >> 0x11;
  }
  else {
    iVar3 = -((int)(uVar1 << 0x16) >> 0x11);
    iVar2 = -(int)(short)param_2[2];
  }
  iVar3 = (iVar3 - *param_1) * param_4;
  if (iVar3 < 0) {
    iVar3 = iVar3 + 0xfff;
  }
  iVar4 = ((iVar5 * 0x1000 >> 0x16) * 0x20 - (int)param_1[1]) * param_4;
  *param_1 = *param_1 + (short)(iVar3 >> 0xc);
  if (iVar4 < 0) {
    iVar4 = iVar4 + 0xfff;
  }
  iVar5 = ((iVar5 * 4 >> 0x16) * 0x20 - (int)param_1[2]) * param_4;
  param_1[1] = param_1[1] + (short)(iVar4 >> 0xc);
  if (iVar5 < 0) {
    iVar5 = iVar5 + 0xfff;
  }
  param_4 = (iVar2 * 2 - (int)param_1[3]) * param_4;
  param_1[2] = param_1[2] + (short)(iVar5 >> 0xc);
  if (param_4 < 0) {
    param_4 = param_4 + 0xfff;
  }
  param_1[3] = param_1[3] + (short)(param_4 >> 0xc);
  return;

}

void anim_lerp_aux(byte *param_1,int param_2,int param_3,int param_4)
{
  int iVar1;
  
  iVar1 = (param_2 << 0x10) >> 0xf;
  if (param_3 != 0) {
    iVar1 = -iVar1;
  }
  param_4 = (iVar1 - *(short *)(param_1 + 6)) * param_4;
  if (param_4 < 0) {
    param_4 = param_4 + 0xfff;
  }
  *(short *)(param_1 + 6) = *(short *)(param_1 + 6) + (short)(param_4 >> 0xc);
  return;

}

void anim_lerp_packed3(short *param_1,ushort *param_2,int param_3,int param_4)
{
  ushort uVar1;
  int iVar2;
  int iVar3;
  {
    extern void decomp_trace_record(uint8_t,uint32_t,uint32_t,uint8_t,uint32_t);
    uint32_t _gd = (uint32_t)guest_of(param_1);
    if (_gd >= 0x801d0000u && _gd < 0x80200000u)
      decomp_trace_record(53, _gd, (uint32_t)guest_of(param_2), 3, 0);
  }
  
  iVar2 = (int)((uint)*param_2 << 0x10) >> 0x18;
  iVar3 = iVar2 * 0x10;
  uVar1 = param_2[1];
  if (param_3 != 0) {
    iVar3 = iVar2 * -0x10;
  }
  iVar3 = (iVar3 - *param_1) * param_4;
  if (iVar3 < 0) {
    iVar3 = iVar3 + 0xfff;
  }
  iVar2 = (((int)((uint)uVar1 << 0x18) >> 0x14) - (int)param_1[1]) * param_4;
  *param_1 = *param_1 + (short)(iVar3 >> 0xc);
  if (iVar2 < 0) {
    iVar2 = iVar2 + 0xfff;
  }
  param_4 = (((int)((uint)uVar1 << 0x10) >> 0x18) * 0x10 - (int)param_1[2]) * param_4;
  param_1[1] = param_1[1] + (short)(iVar2 >> 0xc);
  if (param_4 < 0) {
    param_4 = param_4 + 0xfff;
  }
  param_1[2] = param_1[2] + (short)(param_4 >> 0xc);
  return;

}


undefined8 vec2_rot_lut(undefined4 param_1,int param_2)
{
  int iVar1;
  int iVar2;
  
  iVar1 = lut_sin_q12(param_1);
  iVar1 = param_2 * iVar1;
  iVar2 = lut_cos_q12(param_1);
  return CONCAT44((iVar1 + 0x800) >> 0xc,(param_2 * -iVar2 + 0x800) >> 0xc);

}

/* ---- event-stream runner + GTE helpers (batch21) ---- */
/* ev_stream_dispatch — event-stream dispatcher (guest-verbatim; a2-pc
 * residual modelled explicitly: handlers get pc = motion+0x84). */
void ev_stream_dispatch(byte *param_1,byte *param_2)
{
  byte *pbVar1;
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
  uint32_t pc_g = *(uint32_t *)(param_2 + 0x84);
  uint32_t _dbsp_n = 0;
  if (*(short *)(param_2 + 0x90) < 1) {
    for (;;) {
      uint8_t op = *(uint8_t *)guest_at(pc_g);
      uint32_t npc_g = 0;
      if (op < 24) {
        pbVar1 = (byte *)PTR_FUN_801b4e68[op]((uint8_t *)param_1,(uint8_t *)param_2, pc_g);
        npc_g = pbVar1 ? guest_of(pbVar1) : 0;
      } else {
        /* Opcodes >=24 dispatch through the guest handler table; try the
         * clean-room port first (call_guest4 -> clean_try_dispatch), with
         * the same generated fallback br2_call_generated used. */
        uint32_t h = *(uint32_t *)guest_at(0x801b4e68u + (uint32_t)op * 4);
        extern uint32_t call_guest4(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
        npc_g = call_guest4(h, br2_gaddr(param_1), br2_gaddr(param_2), pc_g, 0);
        pbVar1 = npc_g ? (byte *)guest_at(npc_g) : (byte *)0;
      }
      { extern void decomp_trace_record(uint8_t,uint32_t,uint32_t,uint8_t,uint32_t);
        decomp_trace_record(60, pc_g, (uint32_t)br2_gaddr(param_2), op, npc_g); }
      if (npc_g == 0) break;
      *(uint32_t *)(param_2 + 0x84) = npc_g;   /* guest sw @0x8017EAF4 */
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
    *(short *)(param_2 + 0x90) = *(short *)(param_2 + 0x90) - *(short *)(param_2 + 0xce);
  }
  return;
}

void part_matrix_compute(undefined4 *param_1,byte *param_2,short *param_3,undefined4 param_4)
{
  uint uVar1;
  undefined2 uVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  undefined4 unaff_s0;
  undefined4 unaff_s1;
  undefined4 unaff_s2;
  undefined4 unaff_s3;
  undefined4 unaff_s4;
  undefined4 unaff_s5;
  undefined4 unaff_s6;
  int iVar10;
  undefined4 unaff_retaddr;
  iVar3 = (int)*param_3;
  iVar4 = (int)param_3[1];
  iVar6 = (int)param_3[2];
  _DAT_1f8003e0 = unaff_s6;
  _DAT_1f8003e4 = unaff_s5;
  _DAT_1f8003e8 = unaff_s4;
  _DAT_1f8003ec = unaff_s3;
  _DAT_1f8003f0 = unaff_s2;
  _DAT_1f8003f4 = unaff_s1;
  _DAT_1f8003f8 = unaff_s0;
  _DAT_1f8003fc = unaff_retaddr;
  FUN_801851d8();
  iVar10 = 0x1000 - iVar4;
  uVar5 = 0;
  iVar7 = iVar3;
  iVar9 = iVar6;
  FUN_801851d8();
  iVar8 = -iVar9 * iVar7;
  setCopReg(2,0x4800,-iVar9);
  setCopReg(2,0x5000,iVar7);
  setCopReg(2,0x5800,uVar5);
  _DAT_1f8003a0 = trig_cos(param_4);
  copFunction(2,0xa00428);
  iVar7 = getCopReg(2,0xc800);
  iVar9 = getCopReg(2,0xd000);
  getCopReg(2,0xd800);
  setCopReg(2,0x4000,iVar10);
  setCopReg(2,0x4800,iVar7 >> 0xc);
  setCopReg(2,0x5000,iVar9 >> 0xc);
  setCopReg(2,0x5800,iVar8 >> 0xc);
  _DAT_1f8003b0 = (undefined2)_DAT_1f8003a0;
  _DAT_1f8003a8 = 0x1000;
  copFunction(2,0x190003d);
  _DAT_1f8003ac = trig_sin(param_4);
  _DAT_1f8003a4 = -_DAT_1f8003ac & 0xffff;
  iVar8 = getCopReg(2,0xc800);
  iVar7 = getCopReg(2,0xd000);
  iVar9 = getCopReg(2,0xd800);
  _DAT_1f8003c8 = (short)iVar4;
  _DAT_1f8003d0 = (short)(iVar7 >> 0xc) + _DAT_1f8003c8;
  _DAT_1f8003c0 = (short)(iVar8 >> 0xc) + _DAT_1f8003c8;
  _DAT_1f8003c2 = (short)iVar3;
  _DAT_1f8003c4 = (undefined2)(iVar9 >> 0xc);
  _DAT_1f8003ce = (short)iVar6;
  _DAT_1f8003c6 = -_DAT_1f8003c2;
  _DAT_1f8003ca = -_DAT_1f8003ce;
  _DAT_1f8003cc = _DAT_1f8003c4;
  gte_mat_vec_swap(param_2,&DAT_1f8003a0,&DAT_1f8003c0);
  uVar2 = _DAT_1f8003b0;
  iVar8 = _DAT_1f8003ac;
  uVar5 = _DAT_1f8003a8;
  uVar1 = _DAT_1f8003a4;
  *param_1 = _DAT_1f8003a0;
  param_1[1] = uVar1;
  param_1[2] = uVar5;
  param_1[3] = iVar8;
  *(undefined2 *)(param_1 + 4) = uVar2;
  return;

}

uint * matrix_vec3_apply(undefined4 *param_1,uint *param_2)
{
  uint uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  setCopControlWord(2,0,*param_1);
  setCopControlWord(2,0x800,param_1[1]);
  setCopControlWord(2,0x1000,param_1[2]);
  setCopControlWord(2,0x1800,param_1[3]);
  setCopControlWord(2,0x2000,param_1[4]);
  setCopReg(2,0,(uint)(ushort)*param_2 | param_2[1] & 0xffff0000);
  setCopReg(2,0x800,param_2[3]);
  copFunction(2,0x486012);
  uVar1 = getCopReg(2,0x4800);
  iVar2 = getCopReg(2,0x5000);
  uVar3 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint)*(ushort *)((char *)param_2 + 2) | param_2[2] << 0x10);
  setCopReg(2,0x800,(int)(short)*(ushort *)((char *)param_2 + 0xe));
  copFunction(2,0x486012);
  iVar4 = getCopReg(2,0x4800);
  uVar5 = getCopReg(2,0x5000);
  iVar6 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint)(ushort)param_2[1] | param_2[2] & 0xffff0000);
  setCopReg(2,0x800,param_2[4]);
  copFunction(2,0x486012);
  *param_2 = iVar4 << 0x10 | uVar1 & 0xffff;
  param_2[3] = iVar6 << 0x10 | uVar3 & 0xffff;
  uVar1 = getCopReg(2,0x4800);
  iVar4 = getCopReg(2,0x5000);
  param_2[1] = uVar1 & 0xffff | iVar2 << 0x10;
  param_2[2] = iVar4 << 0x10 | uVar5 & 0xffff;
  uVar1 = getCopReg(2,0xb);
  param_2[4] = uVar1;
  return param_2;

}


/* dir_rotate_acc/8d468 — verified against disasm 0x8018d444-0x8018d4f8.
 * Both tail-call dir_rotate_wrap (2D direction rotate+wrap, returns
 * quotients in $v1/$t4 packed into the u64 port's hi/lo halves).
 * 8d444: only when a1!=0, ACCUMULATES into out pair (a3).
 * 8d468: unconditional OVERWRITE of out pair. */
void dir_rotate_acc(int param_1,int param_2,byte *param_3,byte *param_4)
{
  if (param_2 != 0) {
    short sVar1 = *(short *)param_4;
    short sVar2 = *(short *)(param_4 + 4);
    unsigned long long r = dir_rotate_wrap(param_1,param_2,(short *)param_3);
    *(short *)param_4 = (short)(sVar1 + (short)(r >> 32));
    *(short *)(param_4 + 4) = (short)(sVar2 + (short)((uint32_t)r >> 16));
  }
}

void dir_rotate_set(int param_1,int param_2,byte *param_3,byte *param_4)
{
  unsigned long long r = dir_rotate_wrap(param_1,param_2,(short *)param_3);
  *(short *)param_4 = (short)(r >> 32);
  *(short *)(param_4 + 4) = (short)((uint32_t)r >> 16);
}
