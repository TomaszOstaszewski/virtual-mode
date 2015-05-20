klibc_stdio_DIR         :=./klibc/stdio/
klibc_stdio_FILES       :=\
 printf.c  \
 putchar.c \
 puts.c    \

klibc_stdio_INCLUDES:=\
 -I./drivers/vga/   \
 -I./klibc/include/ \
