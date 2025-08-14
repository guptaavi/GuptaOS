#include "kmain.h"
#include "../include/idt.h"
#include "../include/types.h"
#include "../include/multiboot.h"
#include "../include/paging.h"
#include "../include/frame_allocator.h"
#include "../include/kmalloc.h"
#include "../include/io.h"

void print_string(const char *s, int row, int col) {
    // Print to screen
    char *videomemptr = (char *)0xb8000;
    unsigned int offset = (row * 80 + col) * 2;
    unsigned int j = 0;
    while (s[j] != '\0') {
        videomemptr[offset] = s[j];
        videomemptr[offset + 1] = 0x02;
        j++;
        offset += 2;
    }

    // Print to serial
    j = 0;
    while (s[j] != '\0') {
        serial_write(s[j]);
        j++;
    }
    serial_write('\n');
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

void print_hex(uint32_t value, int row, int col) {
    char hex_digits[] = "0123456789ABCDEF";
    char buffer[11]; // "0x" + 8 hex digits + null terminator
    buffer[0] = '0';
    buffer[1] = 'x';
    buffer[10] = '\0';

    for (int i = 9; i >= 2; i--) {
        buffer[i] = hex_digits[value % 16];
        value /= 16;
    }
    print_string(buffer, row, col);
}


void kmain(uint32_t multiboot_magic, uint32_t multiboot_info_addr) {
    serial_init();

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

    // Temporarily disable interrupts for stable output during tests
    __asm__ __volatile__ ("cli");
    print_string("Interrupts temporarily disabled.", 3, 0);

    print_string("Kernel running!", 2, 0);

    if (multiboot_magic != 0x2BADB002) {
        print_string("Error: Invalid Multiboot magic number!", 4, 0);
        while(1);
    } else {
        print_string("Multiboot magic number OK.", 4, 0);
    }

    multiboot_info_t *mbi = (multiboot_info_t *)multiboot_info_addr;

    print_string("Multiboot info addr: ", 5, 0);
    print_hex(multiboot_info_addr, 5, 21);
    print_string("Flags from addr: ", 6, 0);
    print_hex(*((uint32_t*)multiboot_info_addr), 6, 19);

    print_string("Multiboot magic: ", 7, 0);
    print_hex(multiboot_magic, 7, 19);
    print_string("Multiboot flags: ", 8, 0);
    print_hex(mbi->flags, 8, 19);

    init_paging();
    print_string("Paging initialized.", 9, 0);

    init_frame_allocator(mbi);
    print_string("Frame allocator initialized.", 10, 0);

    // Test map_page
    print_string("Testing map_page...", 11, 0);
    uint32_t test_virtual_address = 0xC0000000; // A high virtual address
    uint32_t test_physical_address = alloc_frame(); // Allocate a physical frame
    if (test_physical_address == 0) {
        print_string("Failed to allocate physical frame for map_page test.", 11, 20);
        while(1);
    }
    print_string("Allocated physical frame for map_page test at: ", 12, 0);
    print_hex(test_physical_address, 12, 50);

    map_page(test_virtual_address, test_physical_address);
    print_string("map_page called.", 13, 0);

    // Try to write to the mapped virtual address
    print_string("Writing to mapped address...", 14, 0);
    char *test_ptr_v = (char *)test_virtual_address;
    *test_ptr_v = 'A';
    *(test_ptr_v + 1) = 'B';
    *(test_ptr_v + 2) = 'C';
    *(test_ptr_v + 3) = 'D';
    print_string("Write complete.", 14, 29);

    // Test kmalloc and kfree
    print_string("--- KMALLOC/KFREE TESTS START ---", 16, 0);

    // Test 1: Allocate a small block
    void *ptr1 = kmalloc(100);
    if (ptr1 != 0) {
        print_string("Allocated 100 bytes at: ", 12, 0);
        print_hex((uint32_t)ptr1, 12, 25);
        kfree(ptr1);
        print_string("Freed 100 bytes. ", 13, 0);
    } else {
        print_string("Failed to allocate 100 bytes.", 12, 0);
    }

    // Test 2: Allocate a larger block
    void *ptr2 = kmalloc(4096 * 2); // 2 pages
    if (ptr2 != 0) {
        print_string("Allocated 8192 bytes (2 pages) at: ", 15, 0);
        print_hex((uint32_t)ptr2, 15, 37);
        kfree(ptr2);
        print_string("Freed 8192 bytes. ", 16, 0);
    } else {
        print_string("Failed to allocate 8192 bytes.", 15, 0);
    }

    // Test 3: Allocate multiple blocks
    void *ptrs[5];
    print_string("Allocating 5 x 512 bytes: ", 18, 0);
    for (int i = 0; i < 5; i++) {
        ptrs[i] = kmalloc(512);
        if (ptrs[i] != 0) {
            print_string("  ptr[", 19 + i, 0);
            char num_str[5];
            itoa(i, num_str);
            print_string(num_str, 19 + i, 6);
            print_string("] at: ", 19 + i, 8);
            print_hex((uint32_t)ptrs[i], 19 + i, 15);
        } else {
            print_string("  Failed to allocate ptr[", 19 + i, 0);
            char num_str[5];
            itoa(i, num_str);
            print_string(num_str, 19 + i, 26);
            print_string("].", 19 + i, 27);
        }
    }

    print_string("Freeing multiple blocks: ", 25, 0);
    for (int i = 0; i < 5; i++) {
        if (ptrs[i] != 0) {
            kfree(ptrs[i]);
            print_string("  Freed ptr[", 26 + i, 0);
            char num_str[5];
            itoa(i, num_str);
            print_string(num_str, 26 + i, 11);
            print_string("].", 26 + i, 12);
        }
    }

    // Test 4: Test for allocation failure (requesting more memory than available)
    print_string("Testing large allocation failure...", 32, 0);
    void *large_ptr = kmalloc(0xFFFFFFFF); // Request a very large amount
    if (large_ptr == 0) {
        print_string("Large allocation failed as expected.", 33, 0);
    } else {
        print_string("Large allocation succeeded unexpectedly at: ", 33, 0);
        print_hex((uint32_t)large_ptr, 33, 45);
        kfree(large_ptr);
    }

    print_string("--- KMALLOC/KFREE TESTS END ---", 35, 0);

    // Re-enable interrupts
    __asm__ __volatile__ ("sti");
    print_string("Interrupts re-enabled.", 31, 0);

    // --- MEMORY DUMP DEBUG CODE ---
    print_string("--- DUMPING MULTIBOOT STRUCT (Offset: Value) ---", 0, 0);
    for (int i = 0; i < 24; i++) {
        uint32_t *ptr = (uint32_t *)multiboot_info_addr;
        uint32_t val = ptr[i];
        char offset_str[5];

        itoa(i * 4, offset_str);
        print_string(offset_str, 2 + i, 0);
        print_string(": ", 2 + i, 4);
        print_hex(val, 2 + i, 6);
    }
    // --- END DEBUG CODE ---

    while (1) {
    }
}
