#ifndef INTRISICS_H
#define INTRISICS_H

#if !defined __cplusplus
#include <stdbool.h>
#endif
#include <stdint.h>

static inline uint32_t read_cr0(void)
{
    uint32_t val;
    asm volatile ( "mov %%cr0, %0" : "=r"(val) );
    return val;
}

static inline uint32_t read_cr3(void)
{
    uint32_t val;
    asm volatile ( "mov %%cr3, %0" : "=r"(val) );
    return val;
}

static inline bool are_interrupts_enabled(void)
{
    unsigned long flags;
    asm volatile ( "pushf\n\t"
                   "pop %0"
                   : "=g"(flags) );
    return flags & (1 << 9);
}

#endif /* INTRISICS_H */
