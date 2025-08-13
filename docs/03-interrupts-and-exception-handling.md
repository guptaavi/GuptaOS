# Chapter 3: Interrupts and Exception Handling

## 3.1. Introduction to Interrupts and Exceptions

Interrupts and exceptions are crucial mechanisms in an operating system that allow the CPU to respond to events. An **interrupt** is an asynchronous event, typically from hardware (like a keyboard press or a timer tick), that diverts the CPU from its current task. An **exception** is a synchronous event, usually caused by a program's execution (like a divide-by-zero error or an invalid memory access).

Both interrupts and exceptions cause the CPU to temporarily suspend its current execution, save its state, and jump to a predefined handler routine. This allows the operating system to respond to events, handle errors, and manage hardware.

## 3.2. The Interrupt Descriptor Table (IDT)

The CPU uses a special data structure called the **Interrupt Descriptor Table (IDT)** to find the correct handler for a given interrupt or exception. The IDT is an array of 256 8-byte entries, where each entry (an `IDTEntry`) points to an interrupt service routine (ISR).

### `IDTEntry` Structure (`src/kernel/include/idt.h`)

```c
typedef struct {
    uint16_t offset_low;    // Lower 16 bits of the offset
    uint16_t selector;      // Code segment selector
    uint8_t zero;           // Always 0
    uint8_t type_attr;      // Type and attributes
    uint16_t offset_high;   // Upper 16 bits of the offset
} __attribute__((packed)) IDTEntry;

// Structure for the IDTR (IDT Register)
typedef struct {
    uint16_t limit;         // Size of the IDT - 1
    uint32_t base;          // Base address of the IDT
} __attribute__((packed)) IDTR;
```

To load the IDT, we use the `lidt` assembly instruction, which takes a pointer to an `IDTR` structure. The `IDTR` contains the base address and limit of the IDT.

## 3.3. Minimal Assembly Stubs (`boot.asm`)

Since the CPU directly jumps to an address specified in the IDT, and C functions cannot directly handle the CPU state saving/restoring required for interrupts, we use minimal assembly stubs. These stubs are the actual targets of the IDT entries. Their job is to:
1.  Push the interrupt number onto the stack.
2.  Push a dummy error code (if the interrupt doesn't provide one).
3.  Save all general-purpose registers (`pusha`).
4.  Call a generic C interrupt handler (`isr_handler`).
5.  Restore all general-purpose registers (`popa`).
6.  Return from the interrupt using `iret`.

```assembly
; ... (Multiboot header and start code)

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
; ... (ISR_NOERRCODE and ISR_ERRCODE for interrupts 0-31)

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

; ... (start code and bss section)
```

## 3.4. C-based IDT Setup (`idt.c`)

The `src/kernel/idt/idt.c` file is responsible for defining the IDT array, setting up individual entries, and loading the IDT.

### `set_idt_entry` Function

This function populates a single `IDTEntry` in the `idt_entries` array.

```c
// Function to set an IDT entry
void set_idt_entry(uint8_t entry_num, uint32_t handler_address, uint16_t selector, uint8_t type_attr) {
    idt_entries[entry_num].offset_low = handler_address & 0xFFFF;
    idt_entries[entry_num].selector = selector;
    idt_entries[entry_num].zero = 0;
    idt_entries[entry_num].type_attr = type_attr;
    idt_entries[entry_num].offset_high = (handler_address >> 16) & 0xFFFF;
}
```

### `init_idt` Function

This function initializes the entire IDT.

```c
// Function to initialize the IDT
void init_idt() {
    idt_ptr.limit = sizeof(IDTEntry) * NUM_IDT_ENTRIES - 1;
    idt_ptr.base = (uint32_t)&idt_entries;

    // Clear all IDT entries
    for (int i = 0; i < NUM_IDT_ENTRIES; i++) {
        set_idt_entry(i, 0, 0, 0);
    }

    // Set up handlers for the first 32 interrupts (exceptions)
    set_idt_entry(0, (uint32_t)isr0, 0x08, 0x8E);  // Divide by Zero
    set_idt_entry(1, (uint32_t)isr1, 0x08, 0x8E);  // Debug
    // ... (other exception handlers 2-31)

    // Load the IDT
    __asm__ __volatile__ ("lidt %0" : : "m" (idt_ptr));
}
```

### Generic C Interrupt Handler (`isr_handler`)

This function is the common C entry point for all interrupts. For now, it simply prints a message and sends an End of Interrupt (EOI) signal to the Programmable Interrupt Controller (PIC).

```c
// Generic C interrupt handler
void isr_handler(void) {
    print_string("Interrupt received!\n");
    // For now, just acknowledge the interrupt if it's from the PIC
    // Later, we'll add more sophisticated PIC handling
    outb(0x20, 0x20); // EOI to master PIC
    outb(0xA0, 0x20); // EOI to slave PIC
}
```

## 3.5. Basic I/O for PIC (`io.c`)

To communicate with hardware devices like the PIC, we use port I/O. The `outb` function sends a byte to a specified I/O port, and `inb` reads a byte from a port.

### `src/kernel/include/io.h`

```c
#include "types.h"

void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);
```

### `src/kernel/io/io.c`

```c
#include "io.h"

void outb(uint16_t port, uint8_t value) {
    __asm__ __volatile__ ("outb %1, %w0" : : "a" (value), "Nd" (port));
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__ ("inb %w0, %1" : "=a" (ret) : "Nd" (port));
    return ret;
}
```

## 3.6. Integrating with the Kernel (`kmain.c`)

To enable interrupt handling, we call `init_idt` from our main kernel function and then enable interrupts using the `sti` instruction.

```c
#include "kmain.h"
#include "../include/idt.h" // Include for IDT functions

// ... (print_string function)

void kmain(void) {
    print_string("Initializing IDT...\n");
    init_idt(); // Initialize the IDT

    print_string("Enabling interrupts...\n");
    __asm__ __volatile__ ("sti"); // Enable interrupts

    print_string("Kernel running!\n");

    // Test: Cause a divide-by-zero exception
    // int a = 10 / 0;

    while (1) {
        // Loop indefinitely
    }
}
```

## 3.7. Testing Interrupts

To test if your interrupt handlers are working, you can intentionally cause an exception. For example, uncommenting the line `int a = 10 / 0;` in `kmain.c` will cause a divide-by-zero exception (Interrupt 0). If your IDT is set up correctly, you should see the "Interrupt received!" message printed on the screen.

```