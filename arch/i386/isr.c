#include "isr.h"
#include "stdio.h"

typedef enum e_regs_indexes {
    REGIDX_EFLAGS = 12,
    REGIDX_CS = 11,
    REGIDX_EIP = 10,
    REGIDX_ERRCODE = 9,
    REGIDX_ISRNUM = 8,
    REGIDX_EAX = 7,
    REGIDX_ECX = 6,
    REGIDX_EDX = 5,
    REGIDX_EBX = 4,
    REGIDX_ESP_USELESS = 3,
    REGIDX_EBP = 2,
    REGIDX_ESI = 1,
    REGIDX_EDI = 0,
} e_regs_indexes;

void isr_handler(const uint32_t *p_regs) {
    printf("%s : %p\n"
           "eax: %x "
           "ecx: %x\n"
           "edx: %x "
           "ebx: %x\n"
           "esi: %x "
           "edi: %x\n"
           "ebp: %x "
           "eip: %x\n"
           "cs: %x "
           "eflags: %x\n"
           ,
           __func__, p_regs, p_regs[REGIDX_EAX], p_regs[REGIDX_ECX], p_regs[REGIDX_EDX],
           p_regs[REGIDX_EBX], p_regs[REGIDX_ESI], p_regs[REGIDX_EDI], p_regs[REGIDX_EBP],
           p_regs[REGIDX_EIP], p_regs[REGIDX_CS], p_regs[REGIDX_EFLAGS]);
}

/**
 */
void irq_handler(const uint32_t *p_regs) { printf("%s : %p\n", __func__, p_regs); }
