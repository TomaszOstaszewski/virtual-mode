#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>
#include "compiler_macros.h"

const uint32_t* CDECL get_irq_table(void);
uint32_t CDECL get_irq_table_size(void);
const uint32_t* CDECL get_isr_table(void);
uint32_t CDECL get_isr_table_size(void);

#endif /* INTERRUPT_H */
