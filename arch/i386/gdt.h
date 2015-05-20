#ifndef GDT_H
#define GDT_H

void set_gdt(uint32_t base, uint16_t size);
void set_idt(uint32_t base, uint16_t size);

#endif /* GDT_H */
