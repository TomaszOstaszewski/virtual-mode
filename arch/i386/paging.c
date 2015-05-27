/**
 * @file descriptor-tables.c
 * @brief Implementation of the GDT/IDT setup.
 */
#include "descriptor-tables.h"

#include <stdint.h>
#include <stddef.h>
#include "stdio.h"
#include "string.h"
#include "gdt.h"
#include "interrupt.h"
#include "compiler_macros.h"

#define PDE_ENTRY_ADDR(physaddr) (((physaddr) >> 12) << 0x0C) /*!< Set page table entry address */
#define PDE_ENTRY_PCD(x) (((x)&0x01) << 0x04)                 /*!< Set page cache disabled. */
#define PDE_ENTRY_PWT(x) (((x)&0x01) << 0x03)                 /*!< Set page cache write through.  */
#define PDE_ENTRY_UorS(x) (((x)&0x01) << 0x02)                /*!< Set page user or system. */
#define PDE_ENTRY_RorW(x) (((x)&0x01) << 0x01) /*!< Set page read only or write allowed. */
#define PDE_ENTRY_PRESENT(x) ((x)&0x01)        /*!< Set page entry present. */

#define DEFAULT_SYSTEM_PDE_ENTRY(x) (PDE_ENTRY_ADDR(x) | PDE_ENTRY_PRESENT)

#define GET_PDE_ENTRY_ADDR(x) ((x)&0xfffff000)           /*!< Get page table address. */
#define GET_PDE_ENTRY_ACCESSED(x) (((x) >> 0x05) & 0x01) /*!< Page accessed */
#define GET_PDE_ENTRY_PCD(x) (((x) >> 0x04) & 0x01)      /*!< Page cache disabled? */
#define GET_PDE_ENTRY_PWT(x) (((x) >> 0x03) & 0x01)      /*!< Page cache write through? */
#define GET_PDE_ENTRY_UorS(x) (((x) >> 0x02) & 0x01)     /*!< Page user or system? */
#define GET_PDE_ENTRY_RorW(x) (((x) >> 0x01) & 0x01)     /*!< Page read only or write allowed? */

#define PTE_ENTRY_ADDR(physaddr) (((physaddr>>12) << 0x0C)            /*!< Set page table entry address */
#define PTE_ENTRY_GLOBAL(x) (((x)&0x01) << 0x08)   /*!< Set page table entry address */
#define PTE_ENTRY_PAT(x) (((x)&0x01) << 0x07)      /*!< Set page table entry address */
#define PTE_ENTRY_DIRTY(x) (((x)&0x01) << 0x06)    /*!< Set page table entry address */
#define PTE_ENTRY_ACCESSED(x) (((x)&0x01) << 0x05) /*!< Set page table entry address */
#define PTE_ENTRY_PCD(x) (((x)&0x01) << 0x04)      /*!< Set page cache disabled. */
#define PTE_ENTRY_PWT(x) (((x)&0x01) << 0x03)      /*!< Set page cache write through.  */
#define PTE_ENTRY_UorS(x) (((x)&0x01) << 0x02)     /*!< Set page user or system. */
#define PTE_ENTRY_RorW(x) (((x)&0x01) << 0x01)     /*!< Set page read only or write allowed. */
#define PTE_ENTRY_PRESENT(x) ((x)&0x01)            /*!< Set page entry present. */

#define GET_PTE_ENTRY_ADDR(x) ((x)&0xfffff000)           /*!< Set page table entry address */
#define GET_PTE_ENTRY_GLOBAL(x) (((x) >> 0x08) & 0x01)   /*!< Set page table entry address */
#define GET_PTE_ENTRY_PAT(x) (((x) >> 0x07) & 0x01)      /*!< Set page table entry address */
#define GET_PTE_ENTRY_DIRTY(x) (((x) >> 0x06) & 0x01)    /*!< Set page table entry address */
#define GET_PTE_ENTRY_ACCESSED(x) (((x) >> 0x05) & 0x01) /*!< Set page table entry address */
#define GET_PTE_ENTRY_PCD(x) (((x) >> 0x04) & 0x01)      /*!< Set page cache disabled. */
#define GET_PTE_ENTRY_PWT(x) (((x) >> 0x03) & 0x01)      /*!< Set page cache write through.  */
#define GET_PTE_ENTRY_UorS(x) (((x) >> 0x02) & 0x01)     /*!< Set page user or system. */
#define GET_PTE_ENTRY_RorW(x) (((x) >> 0x01) & 0x01) /*!< Set page read only or write allowed. */
#define GET_PTE_ENTRY_PRESENT(x) ((x)&0x01)          /*!< Set page entry present. */

#define DEFAULT_SYSTEM_PTE_ENTRY(x) (PTE_ENTRY_ADDR(x) | PTE_ENTRY_PRESENT)

/**
 * @brief
 * @details
 */
uint32_t __attribute__((aligned(4096))) s_pde_entries[1024];

/**
 * @brief
 */
uint32_t __attribute__((aligned(4096))) s_pte_entries_lower_4MB[1024];

uint32_t *get_pde_table(void) { return &s_pde_entries[0]; }

void identity_map_first_4Mb(void) {
  memset(s_pde_entries, 0, sizeof(s_pde_entries));
    s_pde_entries[0] = (((uint32_t)&s_pte_entries_lower_4MB[0]) & 0xfffff000) | 0x00000001;
    size_t idx;
    for (idx = 0; idx < sizeof(s_pte_entries_lower_4MB) / sizeof(s_pte_entries_lower_4MB[0]);
         ++idx) {
      s_pte_entries_lower_4MB[idx] = 0x00000001 | (idx << 12);
    }
    s_pde_entries[(0xc0000000>>22)] = (((uint32_t)s_pte_entries_lower_4MB) & 0xfffff000) | 0x00000001;
}

uint32_t map_v_to_p(uint32_t vaddr) {
    uint32_t *cr3;
    asm volatile("mov %%cr3,%0" : "=r"(cr3));
    printf("%s : PDE at %x\n", __func__, (uint32_t)cr3);
    uint32_t pde_idx = vaddr >> 22;
    printf("%s : PDE[%x]=%x\n", __func__, vaddr, pde_idx);
    if (cr3[pde_idx] & 0x1) {
        uint32_t pte_idx = (vaddr & 0x003ff000) >> 12;
        const uint32_t *pte_table = (const uint32_t *)(s_pde_entries[pde_idx] & 0xfffff000);
        if (pte_table[pte_idx] & 0x1) {
          printf("%s : PTE[%x]=%x\n", __func__, pte_idx, pte_table[pte_idx]);
          uint32_t phy_addr;
          phy_addr = (pte_table[pte_idx]&0xfffff000) | (vaddr & 0x00000fff);
          printf("%s : PHY[%x] = %x\n", __func__, vaddr, phy_addr);
        } else {
            printf("%s : PTE entry %x not present!\n", __func__, pte_idx);
        }
    } else {
        printf("%s : s_pde_entries[%x] not present!\n", __func__, pde_idx);
    }
    return pde_idx;
}

void turn_on_paging(void) {
    uint32_t *pde_table = get_pde_table();
    uint32_t cr0;
    /* Set the CR3 register */
    asm volatile("mov %[cr3], %%cr3" : : [cr3] "r"(pde_table) : "memory");
    identity_map_first_4Mb();
    /* Turn on paging */
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
#if 1
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0\n\t"
                 "jmp 1f\n\t"
                 "1: "
                 : : "r"(cr0));
    //    p_table += 0xc0000000;
#endif
    uint8_t *p_table = (uint8_t *)&s_pde_entries[0];
    p_table += 0xc0400000;
    //    map_v_to_p((uint32_t)(p_table + 0xc0000000));
    *p_table = 0x1;
    //    p_table += 0x
    //    map_v_to_p((uint32_t)(p_table + 0xc03fffff - 0x103000));
    //    map_v_to_p((uint32_t)(p_table + 0xc03fffff - 0x103000 + 1));
}
