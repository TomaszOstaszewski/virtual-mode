typedef unsigned int UINT32_T;
typedef unsigned char UINT8_T;

typedef struct a_out_syms_t {
    UINT32_T tabsize_;
    UINT32_T strsize_;
    UINT32_T addr_;
    UINT32_T reserved_;
} a_out_syms_t;

typedef struct elf_syms_t {
    UINT32_T num_;
    UINT32_T size_;
    UINT32_T addr_;
    UINT32_T shndx_;
} elf_syms_t;

typedef struct mmap_info_t {
    UINT32_T size_;
    UINT32_T base_addr_;
    UINT32_T length_;
    UINT32_T type_;
} mmap_info_t;

typedef struct grub_meminfo_t {
    UINT32_T flags_;
    UINT32_T mem_lower_;
    UINT32_T mem_upper_;
    UINT32_T boot_device_;
    UINT32_T cmdline_;
    union {
        a_out_syms_t a_out_;
        elf_syms_t elf_;
    } syms;
    UINT32_T mmap_len_;
    UINT32_T mmap_addr_;
    
} grub_meminfo_t;

#define MEMINFO_MEM 0x00000001
#define MEMINFO_BOOTDEV 0x00000002
#define MEMINFO_CMDLINE 0x00000004
#define MEMINFO_MODULES 0x00000008
#define MEMINFO_AOUT 0x00000010
#define MEMINFO_ELF 0x00000020
#define MEMINFO_MMAP 0x00000040

void parse_grub_meminfo(grub_meminfo_t const * p_phy_meminfo) {
    if (p_phy_meminfo->flags_ & MEMINFO_MEM) {
    }
    if (p_phy_meminfo->flags_ & MEMINFO_MMAP) {
        mmap_info_t* p_inst = (mmap_info_t*)((UINT8_T*)p_phy_meminfo->mmap_addr_ - 4);
        UINT32_T max_len = p_phy_meminfo->mmap_len_;
        while ((UINT8_T*)p_inst - (UINT8_T*)p_phy_meminfo->mmap_addr_ < max_len) {
            p_inst = (mmap_info_t*)((UINT8_T*)p_inst + p_inst->size_);
        }
    }
    return;
}
