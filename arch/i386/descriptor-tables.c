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

// Each define here is for a specific flag in the descriptor.
// Refer to the intel documentation for a description of what each one does.
#define SEG_DESCTYPE(x) ((x) << 0x04)    // Descriptor type (0 for system, 1 for code/data)
#define SEG_PRES(x) ((x) << 0x07)        // Present
#define SEG_SAVL(x) ((x) << 0x0C)        // Available for system use
#define SEG_LONG(x) ((x) << 0x0D)        // Long mode
#define SEG_SIZE(x) ((x) << 0x0E)        // Size (0 for 16-bit, 1 for 32)
#define SEG_GRAN(x) ((x) << 0x0F)        // Granularity (0 for 1B - 1MB, 1 for 4KB - 4GB)
#define SEG_PRIV(x) (((x)&0x03) << 0x05) // Set privilege level (0 - 3)

#define IDT_DESC_TASK_GATE (0x05)
#define IDT_DESC_INTR_GATE (0x0E)
#define IDT_DESC_TRAP_GATE (0x0F)

#define IGATE_PRES(x) ((x) << 0x0F)       // Interrupt gate present flag
#define IGATE_DPL(x) (((x)&0x03) << 0x0D) // Interrupt descriptor privilege level
#define IGATE_TYPE(x) (((x)&0x1f) << 8)

/**
 * @brief Helper macro that fills in the P, DPL and Type fields of
 * the IDT gate descriptor in a manner specific to the 80386 Interrupt Gate.
 */
#define IDT_INTR_GATE_32BIT (IGATE_PRES(1) | IGATE_DPL(0) | IGATE_TYPE(IDT_DESC_INTR_GATE))

#define SEG_DATA_RD 0x00        // Read-Only
#define SEG_DATA_RDA 0x01       // Read-Only, accessed
#define SEG_DATA_RDWR 0x02      // Read/Write
#define SEG_DATA_RDWRA 0x03     // Read/Write, accessed
#define SEG_DATA_RDEXPD 0x04    // Read-Only, expand-down
#define SEG_DATA_RDEXPDA 0x05   // Read-Only, expand-down, accessed
#define SEG_DATA_RDWREXPD 0x06  // Read/Write, expand-down
#define SEG_DATA_RDWREXPDA 0x07 // Read/Write, expand-down, accessed
#define SEG_CODE_EX 0x08        // Execute-Only
#define SEG_CODE_EXA 0x09       // Execute-Only, accessed
#define SEG_CODE_EXRD 0x0A      // Execute/Read
#define SEG_CODE_EXRDA 0x0B     // Execute/Read, accessed
#define SEG_CODE_EXC 0x0C       // Execute-Only, conforming
#define SEG_CODE_EXCA 0x0D      // Execute-Only, conforming, accessed
#define SEG_CODE_EXRDC 0x0E     // Execute/Read, conforming
#define SEG_CODE_EXRDCA 0x0F    // Execute/Read, conforming, accessed

#define GDT_CODE_PL0                                                                               \
    SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) |        \
        SEG_PRIV(0) | SEG_CODE_EXRD

#define GDT_DATA_PL0                                                                               \
    SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) |        \
        SEG_PRIV(0) | SEG_DATA_RDWR

#define GDT_CODE_PL3                                                                               \
    SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) |        \
        SEG_PRIV(3) | SEG_CODE_EXRD

#define GDT_DATA_PL3                                                                               \
    SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) |        \
        SEG_PRIV(3) | SEG_DATA_RDWR

/**
 * @brief The Global Descriptor Table (GDT)
 * @details This table holds segement descirptors, each one of them
 * being 8 byte enitity that describes a given segment.
 * Indexes to this table are being loaded to the segment registers,
 * and this is how the code using CS, DS, and SS segments gets linked
 * with the GDT table.
 */
static uint64_t gdt_table[5];

/**
 * @brief The Interrupt Descriptor Table (IDT)
 */
static uint64_t idt_table[256];

/**
 * @brief Creates a single descriptor entry.
 *
 * @param base
 * @param limit
 * @param flag
 *
 * @return Returns an 8 byte descriptor.
 */
static uint64_t STDCALL create_gdt_descriptor(uint32_t base, uint32_t limit, uint16_t flag) {
    uint64_t descriptor = 0;

    // Create the high 32 bit segment
    descriptor = limit & 0x000F0000;         // set limit bits 19:16
    descriptor |= (flag << 8) & 0x00F0FF00;  // set type, p, dpl, s, g, d/b, l and avl fields
    descriptor |= (base >> 16) & 0x000000FF; // set base bits 23:16
    descriptor |= base & 0xFF000000;         // set base bits 31:24

    // Shift by 32 to allow for low part of segment
    descriptor <<= 32;

    // Create the low 32 bit segment
    descriptor |= base << 16;         // set base bits 15:0
    descriptor |= limit & 0x0000FFFF; // set limit bits 15:0

    printf("0x%x%x\n", (uint32_t)(descriptor >> 32), (uint32_t)(descriptor & 0xffffffff));
    return descriptor;
}

static void STDCALL get_gdt(uint32_t *p_base, uint16_t *p_offset) {
    uint8_t buf[sizeof(uint16_t) + sizeof(uint32_t)];
    asm volatile("sgdt %[result]" : [result] "=m"(buf));
    memcpy(p_offset, &buf[0], sizeof(*p_offset));
    memcpy(p_base, &buf[2], sizeof(*p_base));
}

static void STDCALL get_idt(uint32_t *p_base, uint16_t *p_offset) {
    uint8_t buf[sizeof(uint16_t) + sizeof(uint32_t)] = {
        0xca, 0xfe, 0xba, 0xbe, 0xde, 0xad,
    };
    asm volatile("sidt %[result]" : [result] "=m"(buf));
    memcpy(p_offset, &buf[0], sizeof(*p_offset));
    memcpy(p_base, &buf[2], sizeof(*p_base));
}

uint64_t STDCALL create_idt_intr_gate_desc(uint32_t handler_addr, uint16_t base) {
    uint64_t interrupt_gate_desc;
    interrupt_gate_desc = handler_addr & 0xffff0000;
    interrupt_gate_desc |= IDT_INTR_GATE_32BIT & 0x0000ff00;
    interrupt_gate_desc &= 0xffffff00;
    interrupt_gate_desc <<= 32;
    interrupt_gate_desc |= (base << 16) & 0xffff0000;
    interrupt_gate_desc |= handler_addr & 0x0000ffff;
    printf("%s: %x %x := %x%x\n", __func__, handler_addr, (uint32_t)base,
           (uint32_t)(interrupt_gate_desc >> 32), (uint32_t)(interrupt_gate_desc & 0xffffffff));
    return interrupt_gate_desc;
}

void init_gdt(void) {
    uint32_t base;
    uint16_t offset;
    gdt_table[0] = create_gdt_descriptor(0, 0, 0);
    gdt_table[1] = create_gdt_descriptor(0, 0x000FFFFF, (GDT_CODE_PL0));
    gdt_table[2] = create_gdt_descriptor(0, 0x000FFFFF, (GDT_DATA_PL0));
    gdt_table[3] = create_gdt_descriptor(0, 0x000FFFFF, (GDT_CODE_PL3));
    gdt_table[4] = create_gdt_descriptor(0, 0x000FFFFF, (GDT_DATA_PL3));
    get_gdt(&base, &offset);
    uint64_t *p_base = (uint64_t *)base;
    size_t idx;
    printf("Current gdt base, offset: %x %x\n", base, (uint32_t)offset);
    for (idx = 0; idx < offset / sizeof(uint64_t); ++idx) {
        printf("Current desc: %x%x\n", (uint32_t)(p_base[idx] >> 32),
               (uint32_t)(p_base[idx] & 0xffffffff));
    }
    set_gdt((uint32_t)&gdt_table[0], sizeof(gdt_table));
    get_gdt(&base, &offset);
    printf("new gdt base, offset: %x %x\n", base, (uint32_t)offset);
}

static void STDCALL fill_idt(uint64_t *idt_table, const uint32_t *fptr_table,
                             uint32_t fptr_table_size, uint16_t selector) {
    printf("%s : %x %x\n", __func__, (uint32_t)fptr_table, fptr_table_size);
    size_t idx;
    for (idx = 0; idx < fptr_table_size; ++idx) {
        idt_table[idx] = create_idt_intr_gate_desc(fptr_table[idx], selector);
    }
}

void init_idt(void) {
    memset(&idt_table, 0, sizeof(idt_table));
    /* Interrupts are handled in a segment pointed by 1st selector of the GDT */
    uint16_t cs_selector = 1 * sizeof(uint64_t);
    uint32_t isr_table_size = get_isr_table_size();
    printf("%s : %x %x\n", __func__, (uint32_t)get_isr_table(), isr_table_size);
    fill_idt(&idt_table[0], get_isr_table(), get_isr_table_size(), cs_selector);
    printf("%s : %x %x\n", __func__, (uint32_t)get_irq_table(), get_irq_table_size());
    fill_idt(&idt_table[isr_table_size], get_irq_table(), get_irq_table_size(), cs_selector);

    uint32_t base;
    uint16_t offset;
    get_idt(&base, &offset);
    printf("%s : IDT at base, offset:", __func__);
    printf("%x, %x\n", base, offset);
    set_idt((uint32_t)&idt_table[0], sizeof(uint64_t) * 4);
    asm volatile("INT3");
}
