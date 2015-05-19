/**
 * 
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

// Some nice typedefs, to standardise sizes across platforms.
// These typedefs are written for 32-bit X86.
typedef uint32_t	u32int;
typedef int32_t		s32int;
typedef uint16_t 	u16int;
typedef int16_t 	s16int;
typedef uint8_t 	u8int;
typedef int8_t 		s8int;

void outb(u16int port, u8int value);
u8int inb(u16int port);
u16int inw(u16int port);

inline static void io_wait( void )
{
    // port 0x80 is used for 'checkpoints' during POST.
    // The Linux kernel seems to think it is free for use :-/
    asm volatile( "outb %%al, $0x80"
                  : : "a"(0) );
}


#endif // COMMON_H
