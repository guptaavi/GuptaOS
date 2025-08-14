#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include "types.h"

// Multiboot information structure
typedef struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
} __attribute__((packed,gcc_struct)) multiboot_info_t;

// Multiboot memory map tag structure
typedef struct multiboot_mmap_entry {
    uint32_t size;
    uint32_t addr_low;
    uint32_t addr_high;
    uint32_t len_low;
    uint32_t len_high;
    uint32_t type;
} __attribute__((packed,gcc_struct)) multiboot_mmap_entry_t;

// Multiboot flags
#define MULTIBOOT_FLAG_MEM     0x001
#define MULTIBOOT_FLAG_BOOTDEV 0x002
#define MULTIBOOT_FLAG_CMDLINE 0x004
#define MULTIBOOT_FLAG_MODS    0x008
#define MULTIBOOT_FLAG_AOUT    0x010
#define MULTIBOOT_FLAG_ELF     0x020
#define MULTIBOOT_FLAG_MMAP    0x040
#define MULTIBOOT_FLAG_DRIVES  0x080
#define MULTIBOOT_FLAG_CONFIG  0x100
#define MULTIBOOT_FLAG_LDRNAME 0x200
#define MULTIBOOT_FLAG_APM     0x400
#define MULTIBOOT_FLAG_VBE     0x800

#endif // MULTIBOOT_H