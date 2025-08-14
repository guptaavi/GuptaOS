#ifndef IO_H
#define IO_H

#include "types.h"

void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);

// Serial port functions
void serial_init();
void serial_write(char c);

#endif // IO_H