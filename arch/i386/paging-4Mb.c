/**
 *
 */
#include <stdint.h>
#include "stdio.h"

uint32_t pde_4Mb_table[1024];

/**********************************************************************************/
/* Currently the stack pointer register (esp) points at anything and using it may */
/* cause massive harm. Instead, we'll provide our own stack. We will allocate     */
/* room for a small temporary stack by creating a symbol at the bottom of it,     */
/* then allocating 16384 bytes for it, and finally creating a symbol at the top.  */
/**********************************************************************************/
uint32_t temp_stack[4096];

#define VIRT_TO_PHY(x) ((uint8_t *)(x) - 0xc0000000)

/**
 *
 */
void __attribute__((noreturn)) __attribute__((optimize("-fomit-frame-pointer")))
setup_4Mb_initial_paging(void) {
    /*********************************************************************/
    /* Init the page table so that both 0-4 Mb range is identity mapped  */
    /* and the 3Gb - 3Gb+4Mb range are mapped mapped to the 0-4 Mb range */
    /* as well                                                           */
    /*********************************************************************/
    uint32_t *phy_addr_pde_table = (uint32_t *)VIRT_TO_PHY(&pde_4Mb_table[0]);
    phy_addr_pde_table[0] = 0x00000083;
    phy_addr_pde_table[0xc0000000 >> 22] = 0x00000083;
    // Setup 4Mb paging
    uint32_t cr_reg;
    // Write CR3 register with PHY address of the page table
    asm volatile("mov %[val], %%cr3" : : [val] "r"(phy_addr_pde_table));
    // Enable 4Mb paging in CR4
    asm volatile("mov %%cr4, %[val]" : [val] "=r"(cr_reg));
    cr_reg |= 0x00000010;
    asm volatile("mov %[val],%%cr4" : : [val] "r"(cr_reg));

    // Finally, enable paging
    asm volatile("mov %%cr0, %[val]" : [val] "=r"(cr_reg));
    cr_reg |= 0x80000000;
    asm volatile("mov %[val],%%cr0\n\t" : : [val] "r"(cr_reg));
    /* Setup the stack frame */
    asm volatile("mov %[val],%%esp" : : [val] "r"(&temp_stack[4096]));

    /* Jump into the higher half                      */
    /* Inline jmp got translated into a relative jump */
    /* and here we want an absolute jump              */
    asm volatile("lea higher_half_cont,%%ecx\n\t"
                 "jmp *%%ecx\n\t" : : : "ecx");
    /* Make compiler happy */
    for (;;) {
        ;
    }
}

/**
 *
 */
void higher_half_cont(void) {
    /* Invalidate page 0 */
    asm volatile("invlpg 0");
    printf("Hello, paged world!");
    /*********************************/
    /* Now you can set up a new GDT  */
    /* and a new IDT.                */
    /*********************************/
    for (;;) {
        ;
    }

    asm volatile("hlt");
}
