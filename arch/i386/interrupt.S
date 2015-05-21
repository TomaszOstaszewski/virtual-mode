; This macro creates a stub for an ISR which does not
; pass any error code.
%macro ISR_NOERRCODE 1
isr__%1:
    cli
    push byte 0
    push byte %1
    jmp isr_common_stub
%endmacro

; This macro creates a stub for an ISR which passes it's own
; error code.
%macro ISR_ERRCODE 1
isr__%1:
    cli                         ; Disable interrupts.
    push byte %1                ; Push the interrupt number
    jmp isr_common_stub
%endmacro

; This macro creates a stub for an IRQ - the first parameter is
; the IRQ number, the second is the ISR number it is remapped to.
%macro IRQ 2
irq__%1:
    cli
    push byte 0
    push byte %2
    jmp irq_common_stub
%endmacro

section .rodata
global isr_table
global isr_table_size

isr_table:
        dd isr__0
        dd isr__1
        dd isr__2
        dd isr__3
        dd isr__4
        dd isr__5
        dd isr__6
        dd isr__7
        dd isr__8
        dd isr__9
        dd isr__10
        dd isr__11
        dd isr__12
        dd isr__13
        dd isr__14
        dd isr__15
        dd isr__16
        dd isr__17
        dd isr__18
        dd isr__19
        dd isr__20
        dd isr__21
        dd isr__22
        dd isr__23
        dd isr__24
        dd isr__25
        dd isr__26
        dd isr__27
        dd isr__28
        dd isr__29
        dd isr__30
        dd isr__31
isr_table_size: 
       dd ($ - isr_table)>>2
        
global irq_table
global irq_table_size
irq_table:
        dd irq__0
        dd irq__1
        dd irq__2
        dd irq__3
        dd irq__4
        dd irq__5
        dd irq__6
        dd irq__7
        dd irq__8
        dd irq__9
        dd irq__10
        dd irq__11
        dd irq__12
        dd irq__13
        dd irq__14
        dd irq__15
irq_table_size:
        dd ($ - irq_table)>>2

      
section .text        
        
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

IRQ   0,    32
IRQ   1,    33
IRQ   2,    34
IRQ   3,    35
IRQ   4,    36
IRQ   5,    37
IRQ   6,    38
IRQ   7,    39
IRQ   8,    40
IRQ   9,    41
IRQ  10,    42
IRQ  11,    43
IRQ  12,    44
IRQ  13,    45
IRQ  14,    46
IRQ  15,    47


        
; In isr.c
[EXTERN isr_handler]
[EXTERN irq_handler]

; This is our common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
isr_common_stub:
    pusha                    ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

    mov ax, ds               ; Lower 16-bits of eax = ds.
    push eax                 ; save the data segment descriptor

    mov ax, 0x10  ; load the kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call isr_handler

    pop ebx        ; reload the original data segment descriptor
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    popa                     ; Pops edi,esi,ebp...
    add esp, 8     ; Cleans up the pushed error code and pushed ISR number
    sti
    iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP


; This is our common IRQ stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
irq_common_stub:
   pusha                    ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

   mov ax, ds               ; Lower 16-bits of eax = ds.
   push eax                 ; save the data segment descriptor

   mov ax, 0x10  ; load the kernel data segment descriptor
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax

   call irq_handler

   pop ebx        ; reload the original data segment descriptor
   mov ds, bx
   mov es, bx
   mov fs, bx
   mov gs, bx

   popa                     ; Pops edi,esi,ebp...
   add esp, 8     ; Cleans up the pushed error code and pushed ISR number
   sti
   iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP 

global get_irq_table
get_irq_table:
        mov eax, $irq_table
        ret

        global get_irq_table_size
get_irq_table_size:
        mov eax, [irq_table_size]
        ret

        global get_isr_table
get_isr_table:
        mov eax, $isr_table
        ret

        global get_isr_table_size
get_isr_table_size:
        mov eax, [isr_table_size]
        ret