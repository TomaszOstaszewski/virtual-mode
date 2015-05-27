boot_DIR        :=./arch/i386/
boot_FILES      :=\
 boot.s   \
 gdt.S    \
 crti.s   \
 crtn.s   \
 interrupt.S \
 isr.c \
 descriptor-tables.c \
 paging.c \

boot_INCLUDES:=\
 -I./klibc/include/

