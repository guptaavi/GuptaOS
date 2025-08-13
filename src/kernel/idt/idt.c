#include "idt.h"
#include "io.h"
#include "../main/kmain.h" // For print_string

#define NUM_IDT_ENTRIES 256

IDTEntry idt_entries[NUM_IDT_ENTRIES];
IDTR idt_ptr;

// Function to set an IDT entry
void set_idt_entry(uint8_t entry_num, uint32_t handler_address, uint16_t selector, uint8_t type_attr) {
    idt_entries[entry_num].offset_low = handler_address & 0xFFFF;
    idt_entries[entry_num].selector = selector;
    idt_entries[entry_num].zero = 0;
    idt_entries[entry_num].type_attr = type_attr;
    idt_entries[entry_num].offset_high = (handler_address >> 16) & 0xFFFF;
}

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
    set_idt_entry(2, (uint32_t)isr2, 0x08, 0x8E);  // Non-Maskable Interrupt
    set_idt_entry(3, (uint32_t)isr3, 0x08, 0x8E);  // Breakpoint
    set_idt_entry(4, (uint32_t)isr4, 0x08, 0x8E);  // Overflow
    set_idt_entry(5, (uint32_t)isr5, 0x08, 0x8E);  // Bound Range Exceeded
    set_idt_entry(6, (uint32_t)isr6, 0x08, 0x8E);  // Invalid Opcode
    set_idt_entry(7, (uint32_t)isr7, 0x08, 0x8E);  // Device Not Available
    set_idt_entry(8, (uint32_t)isr8, 0x08, 0x8E);  // Double Fault
    set_idt_entry(9, (uint32_t)isr9, 0x08, 0x8E);  // Coprocessor Segment Overrun
    set_idt_entry(10, (uint32_t)isr10, 0x08, 0x8E); // Invalid TSS
    set_idt_entry(11, (uint32_t)isr11, 0x08, 0x8E); // Segment Not Present
    set_idt_entry(12, (uint32_t)isr12, 0x08, 0x8E); // Stack-Segment Fault
    set_idt_entry(13, (uint32_t)isr13, 0x08, 0x8E); // General Protection Fault
    set_idt_entry(14, (uint32_t)isr14, 0x08, 0x8E); // Page Fault
    set_idt_entry(15, (uint32_t)isr15, 0x08, 0x8E); // Reserved
    set_idt_entry(16, (uint32_t)isr16, 0x08, 0x8E); // x87 FPU Error
    set_idt_entry(17, (uint32_t)isr17, 0x08, 0x8E); // Alignment Check
    set_idt_entry(18, (uint32_t)isr18, 0x08, 0x8E); // Machine Check
    set_idt_entry(19, (uint32_t)isr19, 0x08, 0x8E); // SIMD Floating-Point Exception
    set_idt_entry(20, (uint32_t)isr20, 0x08, 0x8E); // Virtualization Exception
    set_idt_entry(21, (uint32_t)isr21, 0x08, 0x8E); // Control Protection Exception
    // 22-31 are reserved

    // Load the IDT
    __asm__ __volatile__ ("lidt %0" : : "m" (idt_ptr));
}

// Generic C interrupt handler
void isr_handler(void) {
    print_string("Interrupt received!", 6, 0); // Print on row 6
    // For now, just acknowledge the interrupt if it's from the PIC
    // Later, we'll add more sophisticated PIC handling
    outb(0x20, 0x20); // EOI to master PIC
    outb(0xA0, 0x20); // EOI to slave PIC
}