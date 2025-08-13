#include "../include/frame_allocator.h"
#include "../include/types.h"
#include "../include/multiboot.h"

#define PAGE_SIZE 4096

static uint32_t *frames_bitmap;
static uint32_t num_frames;

static void set_frame_bit(uint32_t frame_addr) {
    uint32_t frame_num = frame_addr / PAGE_SIZE;
    frames_bitmap[frame_num / 32] |= (1 << (frame_num % 32));
}

static void clear_frame_bit(uint32_t frame_addr) {
    uint32_t frame_num = frame_addr / PAGE_SIZE;
    frames_bitmap[frame_num / 32] &= ~(1 << (frame_num % 32));
}

static uint8_t test_frame_bit(uint32_t frame_addr) {
    uint32_t frame_num = frame_addr / PAGE_SIZE;
    return (frames_bitmap[frame_num / 32] & (1 << (frame_num % 32))) != 0;
}

void init_frame_allocator(multiboot_info_t *mbi) {
    uint32_t max_addr = 0;
    if (mbi->flags & MULTIBOOT_FLAG_MMAP) {
        multiboot_mmap_entry_t *mmap = (multiboot_mmap_entry_t *)mbi->mmap_addr;
        while ((uint32_t)mmap < mbi->mmap_addr + mbi->mmap_length) {
            if (mmap->type == 1) {
                uint32_t current_end = mmap->addr_low + mmap->len_low;
                if (current_end > max_addr) {
                    max_addr = current_end;
                }
            }
            mmap = (multiboot_mmap_entry_t *)((uint32_t)mmap + mmap->size + 4);
        }
    }

    num_frames = max_addr / PAGE_SIZE;
    frames_bitmap = (uint32_t *)0x200000;

    for (uint32_t i = 0; i < num_frames / 32; i++) {
        frames_bitmap[i] = 0xFFFFFFFF;
    }
    for (uint32_t i = (num_frames / 32) * 32; i < num_frames; i++) {
        set_frame_bit(i * PAGE_SIZE);
    }

    if (mbi->flags & MULTIBOOT_FLAG_MMAP) {
        multiboot_mmap_entry_t *mmap = (multiboot_mmap_entry_t *)mbi->mmap_addr;
        while ((uint32_t)mmap < mbi->mmap_addr + mbi->mmap_length) {
            if (mmap->type == 1) {
                for (uint32_t addr = mmap->addr_low; addr < mmap->addr_low + mmap->len_low; addr += PAGE_SIZE) {
                    clear_frame_bit(addr);
                }
            }
            mmap = (multiboot_mmap_entry_t *)((uint32_t)mmap + mmap->size + 4);
        }
    }

    for (uint32_t addr = (uint32_t)frames_bitmap; addr < (uint32_t)frames_bitmap + (num_frames / 8); addr += PAGE_SIZE) {
        set_frame_bit(addr);
    }

    for (uint32_t addr = 0x100000; addr < 0x200000; addr += PAGE_SIZE) {
        set_frame_bit(addr);
    }
}

uint32_t alloc_frame() {
    for (uint32_t i = 0; i < num_frames / 32; i++) {
        if (frames_bitmap[i] != 0xFFFFFFFF) {
            for (uint32_t j = 0; j < 32; j++) {
                if (!(frames_bitmap[i] & (1 << j))) {
                    uint32_t frame_addr = (i * 32 + j) * PAGE_SIZE;
                    set_frame_bit(frame_addr);
                    return frame_addr;
                }
            }
        }
    }
    return 0;
}

void free_frame(uint32_t addr) {
    clear_frame_bit(addr);
}
