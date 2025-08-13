#include "../include/paging.h"
#include "../main/kmain.h" // For print_string

// Page directory and page tables
static PAGE_DIRECTORY_ENTRY page_directory[1024] __attribute__((aligned(4096)));
static PAGE_TABLE_ENTRY first_page_table[1024] __attribute__((aligned(4096)));

// Function to initialize paging
void init_paging() {
    // Initialize page directory
    for (int i = 0; i < 1024; i++) {
        page_directory[i].present = 0;
        page_directory[i].rw = 1; // Allow writes
        page_directory[i].user = 0; // Kernel-mode only
        page_directory[i].frame = 0;
    }

    // Map the first 4MB of virtual memory to the first 4MB of physical memory
    for (int i = 0; i < 1024; i++) {
        first_page_table[i].present = 1;
        first_page_table[i].rw = 1;
        first_page_table[i].user = 0;
        first_page_table[i].frame = i; // Identity mapping
    }

    // Add the first page table to the page directory
    page_directory[0].present = 1;
    page_directory[0].rw = 1;
    page_directory[0].user = 0;
    page_directory[0].frame = (uint32_t)first_page_table >> 12; // Address of the page table

    // Load the page directory into the CR3 register
    asm volatile("mov %0, %%cr3" :: "r"(page_directory));

    // Enable paging by setting the PG bit in CR0
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000; // Set PG bit
    asm volatile("mov %0, %%cr0" :: "r"(cr0));
}

// Function to map a virtual address to a physical address
void map_page(uint32_t virtual_address, uint32_t physical_address) {
    // This function will be implemented later when we have a frame allocator
}
