#include "../include/paging.h"
#include "../main/kmain.h" // For print_string
#include "../include/frame_allocator.h" // For alloc_frame

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
    uint32_t pd_index = virtual_address >> 22;
    uint32_t pt_index = (virtual_address >> 12) & 0x3FF;

    // Check if the page table is present
    if (!page_directory[pd_index].present) {
        // Allocate a new page table
        uint32_t new_pt_addr = alloc_frame();
        if (new_pt_addr == 0) {
            // Out of memory for new page table!
            return;
        }
        page_directory[pd_index].present = 1;
        page_directory[pd_index].rw = 1;
        page_directory[pd_index].user = 0;
        page_directory[pd_index].frame = new_pt_addr >> 12;

        // Zero out the new page table
        PAGE_TABLE_ENTRY *new_pt = (PAGE_TABLE_ENTRY *)(new_pt_addr);
        for (int i = 0; i < 1024; i++) {
            new_pt[i].present = 0;
        }
    }

    // Get the page table
    PAGE_TABLE_ENTRY *page_table = (PAGE_TABLE_ENTRY *)(page_directory[pd_index].frame << 12);

    // Map the page
    page_table[pt_index].present = 1;
    page_table[pt_index].rw = 1;
    page_table[pt_index].user = 0;
    page_table[pt_index].frame = physical_address >> 12;

    // Invalidate TLB for the mapped virtual address
    asm volatile("invlpg (%0)" :: "r"(virtual_address));
}
