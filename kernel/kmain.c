/**
 * @file kmain.c
 * @brief Entry point for 32 bit kernel
 */
#include <stdint.h>
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

//extern uint64_t* gdt_table;

uint64_t gdt_table[5];

int kernel_main(void) {
    printf("%s : Hello, world!\n", __func__);
    //    printf("Interrupts: %s\n", are_interrupts_enabled() ? "enabled" : "disabled");
    parse_cr3(read_cr3());
    parse_cr0(read_cr0());
    init_gdt();
    init_idt();
    printf("%u %s: %x%x\n",__LINE__,  __func__, (uint32_t)(gdt_table[1] >> 32), (uint32_t)(gdt_table[1]&0xffffffff));
    turn_on_paging();
    uint64_t* rel_idt = &gdt_table[0] + 0xc0000000/sizeof(uint64_t);
    printf("%u %s: %x%x\n",__LINE__,  __func__, (uint32_t)(rel_idt[1] >> 32), (uint32_t)(rel_idt[1]&0xffffffff));
    return 0;
}
