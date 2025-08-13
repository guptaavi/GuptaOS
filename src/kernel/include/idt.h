#ifndef IDT_H
#define IDT_H

#include "types.h"

// Structure for an IDT entry
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

// Function to initialize the IDT
void init_idt();

// Function to set an IDT entry
void set_idt_entry(uint8_t entry_num, uint32_t handler_address, uint16_t selector, uint8_t type_attr);

// External assembly interrupt handlers (stubs)
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

// Generic C interrupt handler
void isr_handler(void);

#endif // IDT_H