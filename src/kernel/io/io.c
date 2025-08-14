#include "io.h"

void outb(uint16_t port, uint8_t value) {
    __asm__ __volatile__ ("outb %0, %1" : : "a" (value), "Nd" (port));
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (ret) : "Nd" (port));
    return ret;
}

#define SERIAL_COM1_BASE 0x3F8

#define SERIAL_DATA_PORT(base)          (base)
#define SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base)   (base + 5)

// Initializes the serial port
void serial_init() {
    // Disable all interrupts
    outb(SERIAL_FIFO_COMMAND_PORT(SERIAL_COM1_BASE), 0x00);

    // Enable DLAB (Divisor Latch Access Bit) to set baud rate
    outb(SERIAL_LINE_COMMAND_PORT(SERIAL_COM1_BASE), 0x80);

    // Set divisor to 12 (for 9600 baud)
    outb(SERIAL_DATA_PORT(SERIAL_COM1_BASE), 0x0C);
    outb(SERIAL_FIFO_COMMAND_PORT(SERIAL_COM1_BASE), 0x00);

    // Set line protocol: 8 bits, no parity, one stop bit (8N1)
    outb(SERIAL_LINE_COMMAND_PORT(SERIAL_COM1_BASE), 0x03);

    // Enable FIFO, clear them, with 14-byte threshold
    outb(SERIAL_FIFO_COMMAND_PORT(SERIAL_COM1_BASE), 0xC7);

    // Enable IRQs, set RTS/DSR
    outb(SERIAL_MODEM_COMMAND_PORT(SERIAL_COM1_BASE), 0x0B);
}

// Check if the serial transmit buffer is empty
int serial_is_transmit_empty() {
   return inb(SERIAL_LINE_STATUS_PORT(SERIAL_COM1_BASE)) & 0x20;
}

// Write a character to the serial port
void serial_write(char c) {
   while (serial_is_transmit_empty() == 0); // Wait for transmit buffer to be empty
   outb(SERIAL_DATA_PORT(SERIAL_COM1_BASE), c);
}