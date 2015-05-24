#include "isr.h"
#include "stdio.h"


typedef enum e_regs_indexes {
  REG_IDX_EFLAGS = 10,
  REG_IDX_CS = 9,
  REG_IDX_EIP = 8,
    REG_IDX_EAX = 7,
    REG_IDX_ECX = 6,
    REG_IDX_EDX = 5,
    REG_IDX_EBX = 4,
    REG_IDX_ESP_USELESS = 3,
    REG_IDX_EBP = 2,
    REG_IDX_ESI = 1,
    REG_IDX_EDI = 0,
} e_regs_indexes;

void isr_handler(const uint32_t *p_regs) {
    printf("%s : %p\n"
           "eax: %x\n"
           "ecx: %x\n"
           "edx: %x\n"
           "ebx: %x\n"
           "esi: %x\n"
           "edi: %x\n"
           , __func__, p_regs,
           p_regs[REG_IDX_EAX],
           p_regs[REG_IDX_ECX],
           p_regs[REG_IDX_EDX],
           p_regs[REG_IDX_EBX],
           p_regs[REG_IDX_ESI],
           p_regs[REG_IDX_EDI]
        );
    
}

/**
 */
void irq_handler(const uint32_t* p_regs)
{
  printf("%s : %p\n", __func__, p_regs);
}
