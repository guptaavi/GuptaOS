bits 32  ;nasm directive - 32 bit
        section .text
         ;multiboot spec
         align 4
         dd 0x1BADB002 ;magic
         dd 0x63 ;flags (page align, memory info, ELF symbols, memory map)
         dd - (0x1BADB002 + 0x63) ;checksum
         
        global start ;to set symbols from source code as global 
        extern kmain ;kmain is the function in C file

        ; Define the code segment selector for the GDT
        CODE_SEG equ 0x08

        ; Interrupt Service Routines (ISRs) - Assembly Stubs
        ; These push the interrupt number and call the C handler
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    push byte 0
    push byte %1
    jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
global isr%1
isr%1:
    push byte %1
    jmp isr_common_stub
%endmacro

ISR_NOERRCODE 0   ; Divide by Zero Exception
ISR_NOERRCODE 1   ; Debug Exception
ISR_NOERRCODE 2   ; Non-Maskable Interrupt
ISR_NOERRCODE 3   ; Breakpoint Exception
ISR_NOERRCODE 4   ; Overflow Exception
ISR_NOERRCODE 5   ; Bound Range Exceeded Exception
ISR_NOERRCODE 6   ; Invalid Opcode Exception
ISR_NOERRCODE 7   ; Device Not Available Exception
ISR_ERRCODE   8   ; Double Fault Exception
ISR_NOERRCODE 9   ; Coprocessor Segment Overrun
ISR_ERRCODE   10  ; Invalid TSS Exception
ISR_ERRCODE   11  ; Segment Not Present Exception
ISR_ERRCODE   12  ; Stack-Segment Fault Exception
ISR_ERRCODE   13  ; General Protection Fault Exception
ISR_ERRCODE   14  ; Page Fault Exception
ISR_NOERRCODE 15  ; Reserved
ISR_NOERRCODE 16  ; x87 FPU Error
ISR_ERRCODE   17  ; Alignment Check Exception
ISR_NOERRCODE 18  ; Machine Check Exception
ISR_NOERRCODE 19  ; SIMD Floating-Point Exception
ISR_NOERRCODE 20  ; Virtualization Exception
ISR_NOERRCODE 21  ; Control Protection Exception
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

extern isr_handler ; C handler for interrupts

isr_common_stub:
    ; Save registers
    pusha
    ; Call the C handler
    call isr_handler
    ; Restore registers
    popa
    ; Return from interrupt
    iret

        start:
         cli ;clear interrupts-- to diable interrupts
         mov esp, stack_space ;set stack pointer
         push ebx ; Push Multiboot info address
         push eax ; Push Multiboot magic number
         call kmain ;calls the main kernel function from c file
         hlt ;halts the CPU
         
        section .bss
        resb 8192 ;8KB memory reserved for the stack
        stack_space:  
