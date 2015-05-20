        section .data
gdtr:
        DW 0
        DD 0
ldtr:
        DW 0
        DD 0
        
section .text
        
        global set_gdt
set_gdt:
        MOV   EAX, [esp + 4]
        MOV   [gdtr + 2], EAX
        MOV   AX, [ESP + 8]
        MOV   [gdtr], AX
        LGDT  [gdtr]
        ; Reload CS register containing code selector:
        JMP   0x08:reload_CS ; 0x08 points at the new code selector
reload_CS:
   ; Reload data segment registers:
   MOV   AX, 0x10 ; 0x10 points at the new data selector
   MOV   DS, AX
   MOV   ES, AX
   MOV   FS, AX
   MOV   GS, AX
   MOV   SS, AX
   RET
        
global set_idt
set_idt:
        MOV EAX, [esp + 4]  ; get the 'base' parameter from the stack
        MOV [ldtr + 2], EAX ; move it to the 'base' member
        MOV AX, [ESP + 8]   ; get the 'limit' parameter off the stack
        MOV [ldtr], AX      ; move it ot the 'limit' member
        LIDT [ldtr]         ; reload IDT register
        RET
