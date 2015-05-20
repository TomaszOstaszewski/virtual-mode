#include <stdio.h>

#if defined(__is_myos_kernel)
#include <kernel/tty.h>
#else
#include "monitor.h"
#endif

int putchar(int ic)
{
#if defined(__is_myos_kernel)
	char c = (char) ic;
	terminal_write(&c, sizeof(c));
#else
	// TODO: You need to implement a write system call.
        monitor_put((char)ic);
#endif
	return ic;
}
