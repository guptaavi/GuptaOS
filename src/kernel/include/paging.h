#ifndef PAGING_H
#define PAGING_H

#include "types.h"

// Page Directory Entry structure
typedef struct {
    uint32_t present : 1;
    uint32_t rw : 1;
    uint32_t user : 1;
    uint32_t pwt : 1;
    uint32_t pcd : 1;
    uint32_t accessed : 1;
    uint32_t dirty : 1;
    uint32_t page_size : 1;
    uint32_t global : 1;
    uint32_t available : 3;
    uint32_t frame : 20; // Page table physical address
} __attribute__((packed)) PAGE_DIRECTORY_ENTRY;

// Page Table Entry structure
typedef struct {
    uint32_t present : 1;
    uint32_t rw : 1;
    uint32_t user : 1;
    uint32_t pwt : 1;
    uint32_t pcd : 1;
    uint32_t accessed : 1;
    uint32_t dirty : 1;
    uint32_t pat : 1;
    uint32_t global : 1;
    uint32_t available : 3;
    uint32_t frame : 20; // Page physical address
} __attribute__((packed)) PAGE_TABLE_ENTRY;

// Function to initialize paging
void init_paging();

// Function to map a virtual address to a physical address
void map_page(uint32_t virtual_address, uint32_t physical_address);

#endif // PAGING_H
