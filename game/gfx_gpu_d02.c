#include <stdint.h>
#include "cpu_state.h"

#include "psx_cyc.h"

extern CPUState *clean_cpu_state(void);
extern void psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);

uint32_t gpu_callback_state_flags(uint32_t a0, uint32_t a1,
                                  uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0;
  (void)a1;
  (void)a2;
  (void)a3;

  if (cpu->pc != 0u) {
    uint32_t continuation = cpu->pc;
    cpu->pc = 0u;
    switch (continuation) {
    case 0x80191AACu: goto block_80191AAC;
    case 0x80191AC0u: goto block_80191AC0;
    case 0x80191AC4u: goto block_80191AC4;
    case 0x80191AD4u: goto block_80191AD4;
    case 0x80191AD8u: goto block_80191AD8;
    case 0x80191AE8u: goto block_80191AE8;
    case 0x80191AECu: goto block_80191AEC;
    case 0x80191B00u: goto block_80191B00;
    case 0x80191B04u: goto block_80191B04;
    case 0x80191B18u: goto block_80191B18;
    case 0x80191B20u: goto block_80191B20;
    case 0x80191B28u: goto block_80191B28;
    case 0x80191B30u: goto block_80191B30;
    case 0x80191B38u: goto block_80191B38;
    case 0x80191B40u: goto block_80191B40;
    case 0x80191B48u: goto block_80191B48;
    case 0x80191B4Cu: goto block_80191B4C;
    case 0x80191B5Cu: goto block_80191B5C;
    case 0x80191B60u: goto block_80191B60;
    case 0x80191B68u: goto block_80191B68;
    case 0x80191B6Cu: goto block_80191B6C;
    default: break;
    }
  }

block_80191AA4:
  cpu->gpr[31] = 0x80191AACu;
  psx_check_interrupts_at(cpu, 0x8019B1E4u);
  cpu->pc = 0x8019B1E4u;
  return cpu->gpr[2];

block_80191AAC:
  psx_store_cycle_barrier();
  cpu->write_byte(cpu->gpr[17] + 18u, (uint8_t)cpu->gpr[2]);
  cpu->gpr[2] &= 0x00FFu;
  cpu->gpr[3] = 1u;
  {
    int branch_taken = cpu->gpr[2] != cpu->gpr[3];
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191AC4u);
      goto block_80191AC4;
    }
    psx_check_interrupts_at(cpu, 0x80191AC0u);
  }

block_80191AC0:
  cpu->gpr[16] |= 0x0008u;
  /* Fall through to the shared byte test. */

block_80191AC4:
  cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[17] + 17u, 2u, 0x20000u);
  {
    int branch_taken = cpu->gpr[2] == cpu->gpr[0];
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191AD8u);
      goto block_80191AD8;
    }
    psx_check_interrupts_at(cpu, 0x80191AD4u);
  }

block_80191AD4:
  cpu->gpr[16] |= 0x0010u;
  /* Fall through to the next byte test. */

block_80191AD8:
  cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[17] + 16u, 2u, 0x20000u);
  {
    int branch_taken = cpu->gpr[2] == cpu->gpr[0];
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191AECu);
      goto block_80191AEC;
    }
    psx_check_interrupts_at(cpu, 0x80191AE8u);
  }

block_80191AE8:
  cpu->gpr[16] |= 0x0020u;
  /* Fall through to the next byte test. */

block_80191AEC:
  cpu->gpr[2] = 0x801C0000u;
  cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[2] - 31965u, 2u, 0x4u);
  {
    int branch_taken = cpu->gpr[2] == cpu->gpr[0];
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191B04u);
      goto block_80191B04;
    }
    psx_check_interrupts_at(cpu, 0x80191B00u);
  }

block_80191B00:
  cpu->gpr[16] |= 0x0080u;
  /* Fall through to the signed halfword tests. */

block_80191B04:
  cpu->gpr[3] = (uint32_t)(int32_t)(int16_t)
      psx_cyc_load_half(cpu, cpu->gpr[17] + 4u, 3u, 0x20000u);
  cpu->gpr[2] = (int32_t)cpu->gpr[3] < 281 ? 1u : 0u;
  {
    int branch_taken = cpu->gpr[2] != cpu->gpr[0];
    /* The branch delay slot always evaluates the next threshold. */
    cpu->gpr[2] = (int32_t)cpu->gpr[3] < 353 ? 1u : 0u;
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191B4Cu);
      goto block_80191B4C;
    }
    psx_check_interrupts_at(cpu, 0x80191B18u);
  }

block_80191B18:
  {
    int branch_taken = cpu->gpr[2] == cpu->gpr[0];
    /* The branch delay slot always evaluates the third threshold. */
    cpu->gpr[2] = (int32_t)cpu->gpr[3] < 401 ? 1u : 0u;
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191B28u);
      goto block_80191B28;
    }
    psx_check_interrupts_at(cpu, 0x80191B20u);
  }

block_80191B20:
  /* The unconditional jump's delay slot sets flag bit 0. */
  cpu->gpr[16] |= 0x0001u;
  psx_check_interrupts_at(cpu, 0x80191B4Cu);
  goto block_80191B4C;

block_80191B28:
  {
    int branch_taken = cpu->gpr[2] == cpu->gpr[0];
    /* The branch delay slot always evaluates the fourth threshold. */
    cpu->gpr[2] = (int32_t)cpu->gpr[3] < 561 ? 1u : 0u;
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191B38u);
      goto block_80191B38;
    }
    psx_check_interrupts_at(cpu, 0x80191B30u);
  }

block_80191B30:
  /* The unconditional jump's delay slot sets flag bit 6. */
  cpu->gpr[16] |= 0x0040u;
  psx_check_interrupts_at(cpu, 0x80191B4Cu);
  goto block_80191B4C;

block_80191B38:
  {
    int branch_taken = cpu->gpr[2] == cpu->gpr[0];
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191B48u);
      goto block_80191B48;
    }
    psx_check_interrupts_at(cpu, 0x80191B40u);
  }

block_80191B40:
  /* The unconditional jump's delay slot sets flag bit 1. */
  cpu->gpr[16] |= 0x0002u;
  psx_check_interrupts_at(cpu, 0x80191B4Cu);
  goto block_80191B4C;

block_80191B48:
  cpu->gpr[16] |= 0x0003u;
  /* Fall through to the remaining field tests. */

block_80191B4C:
  cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[17] + 18u, 2u, 0x20000u);
  cpu->gpr[3] = (uint32_t)(int32_t)(int16_t)
      psx_cyc_load_half(cpu, cpu->gpr[17] + 6u, 3u, 0x20000u);
  {
    int branch_taken = cpu->gpr[2] != cpu->gpr[0];
    /* The branch delay slot always tests the first halfword threshold. */
    cpu->gpr[2] = (int32_t)cpu->gpr[3] < 289 ? 1u : 0u;
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191B60u);
      goto block_80191B60;
    }
    psx_check_interrupts_at(cpu, 0x80191B5Cu);
  }

block_80191B5C:
  cpu->gpr[2] = (int32_t)cpu->gpr[3] < 257 ? 1u : 0u;
  /* Fall through to the branch on the selected threshold. */

block_80191B60:
  {
    int branch_taken = cpu->gpr[2] != cpu->gpr[0];
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191B6Cu);
      goto block_80191B6C;
    }
    psx_check_interrupts_at(cpu, 0x80191B68u);
  }

block_80191B68:
  cpu->gpr[16] |= 0x0024u;
  /* Fall through to the guest-global pointer loads. */

block_80191B6C:
  cpu->gpr[2] = 0x801C0000u;
  cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[2] - 31976u, 2u, 0x4u);
  cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[2] + 16u, 2u, 0x4u);
  cpu->pc = 0x80191B80u;
  return cpu->gpr[2];
}

uint32_t gpu_callback_dispatch(uint32_t a0, uint32_t a1,
                               uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0;
  (void)a1;
  (void)a2;
  (void)a3;

  if (cpu->pc != 0u) {
    uint32_t continuation = cpu->pc;
    cpu->pc = 0u;
    switch (continuation) {
    case 0x80191B88u: goto block_80191B88;
    default: break;
    }
  }

block_80191B80:
  {
    uint32_t target = cpu->gpr[2];
    cpu->gpr[31] = 0x80191B88u;
    cpu->gpr[4] = cpu->gpr[16];
    psx_check_interrupts_at(cpu, target);
    cpu->pc = target;
    return cpu->gpr[2];
  }

block_80191B88:
  cpu->gpr[2] = 8u;
  psx_store_cycle_barrier();
  cpu->write_byte(cpu->gpr[17] + 18u, (uint8_t)cpu->gpr[2]);
  cpu->pc = 0x80191B90u;
  return cpu->gpr[2];
}

uint32_t gpu_callback_descriptor_match(uint32_t a0, uint32_t a1,
                                       uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0;
  (void)a1;
  (void)a2;
  (void)a3;

  if (cpu->pc != 0u) {
    uint32_t continuation = cpu->pc;
    cpu->pc = 0u;
    switch (continuation) {
    case 0x80191BB0u: goto block_80191BB0;
    case 0x80191BC8u: goto block_80191BC8;
    case 0x80191BE0u: goto block_80191BE0;
    case 0x80191BF8u: goto block_80191BF8;
    default: break;
    }
  }

block_80191B90:
  cpu->gpr[4] = 0x801C0000u - 31852u;
  cpu->gpr[2] = psx_cyc_load_half(cpu, cpu->gpr[4], 2u, 0x10u);
  cpu->gpr[3] = (uint32_t)(int32_t)(int16_t)
      psx_cyc_load_half(cpu, cpu->gpr[17] + 8u, 3u, 0x20000u);
  cpu->gpr[2] = (uint32_t)(int32_t)(int16_t)(uint16_t)cpu->gpr[2];
  {
    int branch_taken = cpu->gpr[2] != cpu->gpr[3];
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191C08u);
      cpu->pc = 0x80191C08u;
      return cpu->gpr[2];
    }
    psx_check_interrupts_at(cpu, 0x80191BB0u);
  }

block_80191BB0:
  cpu->gpr[2] = psx_cyc_load_half(cpu, cpu->gpr[4] + 2u, 2u, 0x10u);
  cpu->gpr[3] = (uint32_t)(int32_t)(int16_t)
      psx_cyc_load_half(cpu, cpu->gpr[17] + 10u, 3u, 0x20000u);
  cpu->gpr[2] = (uint32_t)(int32_t)(int16_t)(uint16_t)cpu->gpr[2];
  {
    int branch_taken = cpu->gpr[2] != cpu->gpr[3];
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191C08u);
      cpu->pc = 0x80191C08u;
      return cpu->gpr[2];
    }
    psx_check_interrupts_at(cpu, 0x80191BC8u);
  }

block_80191BC8:
  cpu->gpr[2] = psx_cyc_load_half(cpu, cpu->gpr[4] + 4u, 2u, 0x10u);
  cpu->gpr[3] = (uint32_t)(int32_t)(int16_t)
      psx_cyc_load_half(cpu, cpu->gpr[17] + 12u, 3u, 0x20000u);
  cpu->gpr[2] = (uint32_t)(int32_t)(int16_t)(uint16_t)cpu->gpr[2];
  {
    int branch_taken = cpu->gpr[2] != cpu->gpr[3];
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191C08u);
      cpu->pc = 0x80191C08u;
      return cpu->gpr[2];
    }
    psx_check_interrupts_at(cpu, 0x80191BE0u);
  }

block_80191BE0:
  cpu->gpr[2] = psx_cyc_load_half(cpu, cpu->gpr[4] + 6u, 2u, 0x10u);
  cpu->gpr[3] = (uint32_t)(int32_t)(int16_t)
      psx_cyc_load_half(cpu, cpu->gpr[17] + 14u, 3u, 0x20000u);
  cpu->gpr[2] = (uint32_t)(int32_t)(int16_t)(uint16_t)cpu->gpr[2];
  {
    int branch_taken = cpu->gpr[2] != cpu->gpr[3];
    /* The branch delay slot sets v0 to eight on either path. */
    cpu->gpr[2] = 8u;
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191C08u);
      cpu->pc = 0x80191C08u;
      return cpu->gpr[2];
    }
    psx_check_interrupts_at(cpu, 0x80191BF8u);
  }

block_80191BF8:
  cpu->gpr[3] = psx_cyc_load_byte(cpu, cpu->gpr[17] + 18u, 3u, 0x20000u);
  {
    int branch_taken = cpu->gpr[3] != cpu->gpr[2];
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191E74u);
      cpu->pc = 0x80191E74u;
      return cpu->gpr[2];
    }
    psx_check_interrupts_at(cpu, 0x80191C08u);
    cpu->pc = 0x80191C08u;
    return cpu->gpr[2];
  }
}

uint32_t gpu_callback_resume_next(uint32_t a0, uint32_t a1,
                                  uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0;
  (void)a1;
  (void)a2;
  (void)a3;

  cpu->pc = 0x80191C08u;
  return cpu->gpr[2];
}
