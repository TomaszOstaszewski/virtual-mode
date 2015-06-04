; Declare constants used for creating a multiboot header.

ALIGNMENT   		equ    1
MEMINFO 		equ    1<<1             ; provide memory map
FLAGS   		equ  ALIGNMENT | MEMINFO  ; this is the Multiboot 'flag' field
MAGIC   		equ  0x1BADB002       ; 'magic number' lets bootloader find the header
CHECKSUM 		equ  -(MAGIC + FLAGS) ; checksum of above, to prove we are multiboot

; Declare a header as in the Multiboot Standard. We put this into a special
; section so we can force the header to be in the start of the final program.
; You don't need to understand all these details as it is just magic values that
; is documented in the multiboot standard. The bootloader will search for this
; magic sequence and recognize us as a multiboot kernel.
section .multiboot
align 4
multiboot_header:
        dd MAGIC
        dd FLAGS
        dd CHECKSUM

extern setup_4Mb_initial_paging

global start
start   equ _start - 0xc0000000
        
; The linker script specifies _start as the entry point to the kernel and the
; bootloader will jump to this position once the kernel has been loaded. It
; doesn't make sense to return from this function as the bootloader is gone.
section .text
global _start
_start:
        ; Clear all interrupts
        ; Until we have IDT those are lethal
        cli
        ; A dummy load of the mulitboot_header address
        ; This informs the linker that this section is indeed used
        ; and shall not be garbage collected
        mov eax, $multiboot_header
        ; Calculate the address of the paging routine in the
        ; physical memory
        mov eax, $setup_4Mb_initial_paging
        sub eax, 0xc0000000
        ; Call paging routine - there's no coming back from here
        jmp eax


