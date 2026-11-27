#include <stdint.h>
#include "cpu_state.h"

extern CPUState *clean_cpu_state(void);
extern void     psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);

uint32_t fixed_rsqrt_q12_zero_result(void)
{
  CPUState *cpu = clean_cpu_state();
  uint32_t resume_pc = cpu->gpr[31];

  cpu->gpr[2] = 0;
  psx_check_interrupts_at(cpu, resume_pc);
  cpu->pc = resume_pc;
  return cpu->gpr[2];
}

uint32_t fixed_rsqrt_q12_ex_failure(void)
{
  CPUState *cpu = clean_cpu_state();
  uint32_t resume_pc = cpu->gpr[31];

  cpu->gpr[2] = UINT32_MAX;
  psx_check_interrupts_at(cpu, resume_pc);
  cpu->pc = resume_pc;
  return cpu->gpr[2];
}

uint32_t vec3_s32_h_normalize_store_tail(void)
{
  CPUState *cpu = clean_cpu_state();

  switch (cpu->pc) {
  case 0x80194678u:
    goto block_80194678;
  default: ;
  }

block_8019466C:
  cpu->gpr[7] = cpu->gpr[31];
  cpu->gpr[31] = 0x80194678u;
  psx_check_interrupts_at(cpu, 0x80194690u);
  cpu->pc = 0x80194690u;
  return cpu->gpr[2];

block_80194678:
  cpu->write_half(cpu->gpr[5], (uint16_t)cpu->gpr[8]);
  cpu->write_half(cpu->gpr[5] + 2, (uint16_t)cpu->gpr[9]);
  cpu->write_half(cpu->gpr[5] + 4, (uint16_t)cpu->gpr[10]);
  cpu->gpr[31] = cpu->gpr[7];
  {
    uint32_t resume_pc = cpu->gpr[31];
    psx_check_interrupts_at(cpu, resume_pc);
    cpu->pc = resume_pc;
  }
  return cpu->gpr[2];
}

uint32_t fixed_recip_negative_shift_tail(void)
{
  CPUState *cpu = clean_cpu_state();
  uint32_t resume_pc;

  cpu->gpr[9] = cpu->gpr[0] - cpu->gpr[9];
  resume_pc = cpu->gpr[31];
  cpu->gpr[2] = cpu->gpr[13] >> (cpu->gpr[9] & 0x1Fu);
  psx_check_interrupts_at(cpu, resume_pc);
  cpu->pc = resume_pc;
  return cpu->gpr[2];
}
