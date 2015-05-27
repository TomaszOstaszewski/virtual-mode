/**
 * @file kmain.c
 * @brief Entry point for 32 bit kernel
 */

#include <stdio.h>
#include "descriptor-tables.h"
#include "intrisics.h"
#include "paging.h"

struct multiboot;

void parse_cr0(uint32_t cr0) {
    int paging = cr0 >> 31;
    int cd = (cr0 >> 30) & 0x1;
    int pme = (cr0 >> 0) & 0x1;
    printf("CR0=%x\npg=%x,cd=%x,pme=%x\n", cr0, paging, cd, pme);
}

void parse_cr3(uint32_t cr3) { printf("Page dir at %x\n", cr3); }

int kernel_main(void) {
    printf("%s : Hello, world!\n", __func__);
    //    printf("Interrupts: %s\n", are_interrupts_enabled() ? "enabled" : "disabled");
    parse_cr3(read_cr3());
    parse_cr0(read_cr0());
    init_gdt();
    turn_on_paging();
    //    init_idt();
    return 0;
}
