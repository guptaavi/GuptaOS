#ifndef FRAME_ALLOCATOR_H
#define FRAME_ALLOCATOR_H

#include "types.h"
#include "multiboot.h"

// Function to initialize the frame allocator
void init_frame_allocator(multiboot_info_t *mbi);

// Function to allocate a physical frame
uint32_t alloc_frame();

// Function to free a physical frame
void free_frame(uint32_t addr);

#endif // FRAME_ALLOCATOR_H