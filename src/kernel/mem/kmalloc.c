#include "../include/kmalloc.h"
#include "../include/types.h"
#include "../include/frame_allocator.h"

#define PAGE_SIZE 4096

void *kmalloc(uint32_t size) {
    uint32_t num_pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    uint32_t addr = 0;

    for (uint32_t i = 0; i < num_pages; i++) {
        uint32_t frame_addr = alloc_frame();
        if (frame_addr == 0) {
            return (void *)0;
        }
        if (i == 0) {
            addr = frame_addr;
        }
    }
    return (void *)addr;
}

void kfree(void *ptr) {
    free_frame((uint32_t)ptr);
}
