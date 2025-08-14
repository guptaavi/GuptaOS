#include "../include/frame_allocator.h"
#include "../include/types.h"
#include "../include/multiboot.h"
#include "../main/kmain.h" // For print_string and print_hex

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
    int mmap_entry_row = 19; // Adjusted declaration here
    print_string("--------------------------------------------------------------------------------", 14, 0);
    print_string("Initializing frame allocator...", 15, 0);
    uint32_t max_addr = 0;
    if (mbi->flags & MULTIBOOT_FLAG_MMAP) {
        print_string("  Multiboot memory map found.", 16, 0);
        print_string("  mmap_addr: ", 17, 0);
        print_hex(mbi->mmap_addr, 17, 13);
        print_string("  mmap_length: ", 18, 0);
        print_hex(mbi->mmap_length, 18, 15);
        for (multiboot_mmap_entry_t *mmap = (multiboot_mmap_entry_t *)mbi->mmap_addr;
             (uint32_t)mmap < mbi->mmap_addr + mbi->mmap_length;
             mmap = (multiboot_mmap_entry_t *)((uint32_t)mmap + mmap->size + sizeof(mmap->size))) {

            print_string("    Entry: addr=", mmap_entry_row, 0);
            print_hex(mmap->addr_low, mmap_entry_row, 17);
            print_string(", len=", mmap_entry_row, 28);
            print_hex(mmap->len_low, mmap_entry_row, 35);
            print_string(", type=", mmap_entry_row, 46);
            char type_str[2];
            itoa(mmap->type, type_str);
            print_string(type_str, mmap_entry_row, 53);
            print_string(" (processed)", mmap_entry_row, 55);
            mmap_entry_row++;

            if (mmap->type == 1) { // Available RAM
                uint32_t current_end = mmap->addr_low + mmap->len_low;
                if (current_end > max_addr) {
                    max_addr = current_end;
                }
            }
        }
    }

    print_string("  Max physical address: ", mmap_entry_row, 0);
    print_hex(max_addr, mmap_entry_row, 26);
    mmap_entry_row++;

    num_frames = max_addr / PAGE_SIZE;
    print_string("  Total frames: ", mmap_entry_row, 0);
    char num_frames_str[10];
    itoa(num_frames, num_frames_str);
    print_string(num_frames_str, mmap_entry_row, 16);
    mmap_entry_row++;

    frames_bitmap = (uint32_t *)0x200000; // Place at 2MB
    print_string("  Bitmap placed at: ", mmap_entry_row, 0);
    print_hex((uint32_t)frames_bitmap, mmap_entry_row, 21);
    mmap_entry_row++;

    // Mark all frames as used initially
    print_string("  Marking all frames as used...", mmap_entry_row, 0);
    mmap_entry_row++;
    for (uint32_t i = 0; i < num_frames / 32; i++) {
        frames_bitmap[i] = 0xFFFFFFFF;
    }
    // Clear remaining bits if num_frames is not a multiple of 32
    for (uint32_t i = (num_frames / 32) * 32; i < num_frames; i++) {
        set_frame_bit(i * PAGE_SIZE); // Mark as used
    }
    print_string("  All frames marked used.", mmap_entry_row, 0);
    mmap_entry_row++;

    // Iterate through memory map and mark available frames as free
    if (mbi->flags & MULTIBOOT_FLAG_MMAP) {
        print_string("  Marking available RAM as free...", mmap_entry_row, 0);
        mmap_entry_row++;
        for (multiboot_mmap_entry_t *mmap = (multiboot_mmap_entry_t *)mbi->mmap_addr;
             (uint32_t)mmap < mbi->mmap_addr + mbi->mmap_length;
             mmap = (multiboot_mmap_entry_t *)((uint32_t)mmap + mmap->size + sizeof(mmap->size))) {

            if (mmap->type == 1) { // Available RAM
                for (uint32_t addr = mmap->addr_low; addr < mmap->addr_low + mmap->len_low; addr += PAGE_SIZE) {
                    clear_frame_bit(addr);
                }
            }
        }
    }
    print_string("  Available RAM marked free.", mmap_entry_row, 0);
    mmap_entry_row++;

    // Mark the bitmap pages as used
    print_string("  Marking bitmap pages as used...", mmap_entry_row, 0);
    mmap_entry_row++;
    for (uint32_t addr = (uint32_t)frames_bitmap; addr < (uint32_t)frames_bitmap + (num_frames / 8); addr += PAGE_SIZE) {
        set_frame_bit(addr);
    }
    print_string("  Bitmap pages marked used.", mmap_entry_row, 0);
    mmap_entry_row++;

    // Mark kernel pages as used (assuming kernel is loaded at 1MB and is less than 1MB in size)
    print_string("  Marking kernel pages as used...", mmap_entry_row, 0);
    mmap_entry_row++;
    for (uint32_t addr = 0x100000; addr < 0x200000; addr += PAGE_SIZE) { // 1MB to 2MB
        set_frame_bit(addr);
    }
    print_string("  Kernel pages marked used.", mmap_entry_row, 0);
    mmap_entry_row++;

    print_string("Frame allocator initialization complete.", mmap_entry_row, 0);
}

uint32_t alloc_frame() {
    for (uint32_t i = 0; i < num_frames / 32; i++) {
        if (frames_bitmap[i] != 0xFFFFFFFF) { // Find a dword with at least one free frame
            for (uint32_t j = 0; j < 32; j++) {
                if (!(frames_bitmap[i] & (1 << j))) { // Find a free bit
                    uint32_t frame_addr = (i * 32 + j) * PAGE_SIZE;
                    set_frame_bit(frame_addr);
                    return frame_addr;
                }
            }
        }
    }
    return 0; // No free frames
}

void free_frame(uint32_t addr) {
    clear_frame_bit(addr);
}