#include "isr.h"
#include "stdio.h"

/**
 */
void isr_handler(registers_t regs) { printf("%s : %x \n", __func__, regs.int_no); }

/**
 */
void irq_handler(registers_t regs) { printf("%s : %x \n", __func__, regs.int_no); }
