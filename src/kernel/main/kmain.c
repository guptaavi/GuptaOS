#include "kmain.h"
#include "../include/idt.h"
#include "../include/types.h"
#include "../include/multiboot.h"

void print_string(const char *s, int row, int col) {
    char *videomemptr = (char *)0xb8000;
    unsigned int offset = (row * 80 + col) * 2;

    unsigned int j = 0;
    while (s[j] != '\0') {
        videomemptr[offset] = s[j];
        videomemptr[offset + 1] = 0x02;
        ++j;
        offset = offset + 2;
    }
}

void itoa(int n, char s[]) {
    int i, sign;
    if ((sign = n) < 0)
        n = -n;
    i = 0;
    do {
        s[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    int c, j;
    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

int strlen(const char *s) {
    int i = 0;
    while (s[i] != '\0')
        i++;
    return i;
}

void kmain(uint32_t multiboot_magic, uint32_t multiboot_info_addr) {
    char *videomemptr = (char *)0xb8000;
    unsigned int j = 0;
    while (j < 80 * 25 * 2) {
        videomemptr[j] = ' ';
        videomemptr[j + 1] = 0x02;
        j = j + 2;
    }

    print_string("Initializing IDT...", 0, 0);
    init_idt();

    print_string("Enabling interrupts...", 1, 0);
    __asm__ __volatile__ ("sti");

    print_string("Kernel running!", 2, 0);

    if (multiboot_magic != 0x2BADB002) {
        print_string("Error: Invalid Multiboot magic number!", 4, 0);
        while(1);
    } else {
        print_string("Multiboot magic number OK.", 4, 0);
    }

    while (1) {
    }
}
