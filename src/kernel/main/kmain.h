#ifndef KMAIN_H
#define KMAIN_H

#include "../include/types.h" // Include for uint32_t

void kmain(uint32_t multiboot_magic, uint32_t multiboot_info_addr);
void print_string(const char *s, int row, int col);
void itoa(int n, char s[]); // Add prototype for itoa
int strlen(const char *s); // Add prototype for strlen

#endif // KMAIN_H