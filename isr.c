//
// isr.c -- High level interrupt service routines and interrupt request handlers.
//          Part of this code is modified from Bran's kernel development tutorials.
//          Rewritten for JamesM's kernel development tutorials.
//

#include <string.h>
#include "common.h"
#include "isr.h"
#include "monitor.h"
#include "pic-8259a.h"

isr_t interrupt_handlers[256];

void init_handlers_table(void)
{
	memset(&interrupt_handlers, 0, sizeof(interrupt_handlers));
}

void register_interrupt_handler(u8int n, isr_t handler)
{
    interrupt_handlers[n] = handler;
}

// This gets called from our ASM interrupt handler stub.
void isr_handler(registers_t regs)
{
		monitor_write("recieved interrupt: ");
		monitor_put(' ');
		monitor_write_dec(__LINE__);
		monitor_put(' ');
		monitor_write_hex(regs.int_no);
		monitor_put(' ');
		monitor_write_dec(regs.int_no);
		monitor_put('\n');
		if (interrupt_handlers[regs.int_no] != 0)
		{
				isr_t handler = interrupt_handlers[regs.int_no];
				handler(regs);
				monitor_write_dec(__LINE__);
				monitor_put('\n');
		}
		else
		{
				monitor_write_dec(__LINE__);
				monitor_put('\n');
		}
}

void irq_handler(registers_t regs)
{
		monitor_write("recieved irq: ");
		monitor_put(' ');
		monitor_write_dec(__LINE__);
		monitor_put(' ');
		monitor_write_hex(regs.int_no);
		monitor_put(' ');
		monitor_put(' ');
		monitor_write_dec(regs.int_no);
		monitor_put('\n');
		pic_send_eoi(regs.int_no);

		if (interrupt_handlers[regs.int_no] != 0)
		{
				isr_t handler = interrupt_handlers[regs.int_no];
				handler(regs);
		}
}
