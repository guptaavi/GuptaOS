#ifndef KMAIN_H
#define KMAIN_H

#include "../include/types.h" // Include for uint32_t

void kmain(uint32_t multiboot_magic, uint32_t multiboot_info_addr);
void print_string(const char *s, int row, int col);
void itoa(int n, char s[]); // Add prototype for itoa
int strlen(const char *s); // Add prototype for strlen
void print_hex(uint32_t value, int row, int col);

#endif // KMAIN_H