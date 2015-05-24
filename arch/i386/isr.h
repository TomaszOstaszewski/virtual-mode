//
// isr.h -- Interface and structures for high level interrupt service routines.
//          Part of this code is modified from Bran's kernel development tutorials.
//          Rewritten for JamesM's kernel development tutorials.
//

#if !defined ISR_H
#define ISR_H

#include <stdint.h>

void isr_handler(const uint32_t* regs);
void irq_handler(const uint32_t* regs);

#endif

