//
// isr.h -- Interface and structures for high level interrupt service routines.
//          Part of this code is modified from Bran's kernel development tutorials.
//          Rewritten for JamesM's kernel development tutorials.
//

#if !defined ISR_H
#define ISR_H
#include "common.h"

typedef struct registers
{
    u32int ds;                  // Data segment selector
    u32int edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
    u32int int_no, err_code;    // Interrupt number and error code (if applicable)
    u32int eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
} registers_t;

// Enables registration of callbacks for interrupts or IRQs.
// For IRQs, to ease confusion, use the #defines above as the
// first parameter.
typedef void (*isr_t)(registers_t );

void init_handlers_table(void);
void register_interrupt_handler(u8int n, isr_t handler); 
void isr_handler(registers_t regs);

#define IRQ0 (0x20)
#define IRQ1 (0x21)
#define IRQ2 (0x22)
#define IRQ3 (0x23)
#define IRQ4 (0x24)
#define IRQ5 (0x25)
#define IRQ6 (0x26)
#define IRQ7 (0x27)

#define IRQ8 (0x28)
#define IRQ9 (0x29)
#define IRQa (0x2a)
#define IRQb (0x2b)
#define IRQc (0x2c)
#define IRQd (0x2d)
#define IRQe (0x2e)
#define IRQf (0x2f)

#endif
