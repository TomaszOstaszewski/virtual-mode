; Declare constants used for creating a multiboot header.
	BITS 32
	CPU 586
ALIGNMENT   		equ  1
MEMINFO 		equ  1<<1             ; provide memory map
MMAP                    equ  1 << 6
FLAGS   		equ  ALIGNMENT | MEMINFO | MMAP  ; this is the Multiboot 'flag' field
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
	section .data
	align 4
page_dir_table:
times 1024  dd 0x00000000
page_dir_table_size:	 dd $ - page_dir_table

align 4
	;; 32 kB of stack real estate
stack_bottom:
times 32768 db 0
stack_top:	

g_mem_lower: dd 0
g_mem_upper: dd 0
	
	lower_
extern setup_4Mb_initial_paging
extern higher_half_cont
extern parse_grub_meminfo
global _start
        
; The linker script specifies _start as the entry point to the kernel and the
; bootloader will jump to this position once the kernel has been loaded. It
; doesn't make sense to return from this function as the bootloader is gone.
section .text
_start:
        ;; Clear all interrupts
        ;; Until we have IDT those are lethal
        cli
        ;; A dummy load of the mulitboot_header address
        ;; This informs the linker that this section is indeed used
        ;; and shall not be garbage collected
        mov eax, $multiboot_header
	;; Setup a temporary stack in physical memory
	;; 
	mov esp, stack_top - 0xc0000000
	;; Call the memory information parsing routine
	push ebx
	call parse_grub_meminfo
	;; Prepare page directory entries
	mov eax, 0x83	     ; 0x83 = Page is 4Mb, page is R/W, page present.
	mov [page_dir_table-0xc0000000],eax    ; Setup identity paging for 0-4Mb range
	mov [page_dir_table+768-0xc0000000],eax ; Make [0; 4Mb) range to be mapped into [3Gb; 3Gb+4Mb) range.
	;; Load the Page Directory into CR3
	mov eax, $page_dir_table
	mov cr3, eax
	;; Enable 4Mb pages
	mov eax, cr4
	or eax, 0x00000010
	mov cr4, eax
	;; Finally, enable paging
	mov eax, cr0		; load CR0
	or eax, 0x80000000	; enable paging bit
	align 32
	;; This instruction enables paging
	;; each instruction fetch after this one does use
	;; page directory entries to handle TLB faults.
	mov cr0, eax
	;;  Reload stack pointer with a virtual address
	mov esp, stack_top
	;; Calculate the address of the higher half routine
	;; and jump into it
        lea eax, [higher_half_cont]
	jmp eax
global start
start   equ _start - 0xc0000000


