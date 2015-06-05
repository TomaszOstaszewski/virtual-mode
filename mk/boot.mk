boot_DIR        :=./arch/i386/
boot_FILES      :=\
 boot.s   \
 grub_meminfo.c \
 gdt.S    \
 crti.s   \
 crtn.s   \
 interrupt.S \
 isr.c \
 descriptor-tables.c \
 paging.c \
 paging-4Mb.c \

boot_INCLUDES:=\
 -I./klibc/include/ \
 -I./drivers/vga


