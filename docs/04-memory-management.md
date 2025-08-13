# Chapter 4: Memory Management

Memory management is one of the most critical functions of an operating system. In this chapter, we will implement a paging system, which allows us to use virtual memory, and we will create a memory allocator to manage our physical memory.

## 4.1. Paging and Virtual Memory

Paging is a memory management scheme that allows the CPU to map virtual addresses to physical addresses. This has several advantages:

*   **Isolation:** Each process can have its own virtual address space, preventing it from interfering with other processes or the kernel.
*   **Flexibility:** We can place code and data anywhere in physical memory without changing the virtual addresses that the program sees.
*   **Efficiency:** We can load pages from disk into memory on demand, which means we don't need to load the entire program into memory at once.

We will set up a simple paging system that maps the first few megabytes of our kernel's virtual address space to the corresponding physical addresses.

### Multiboot Information Structure (`multiboot_info_t`)

Before the kernel can manage memory, it needs to know what memory is available in the system. This information is provided by the bootloader (GRUB, in our case) through the Multiboot Information Structure, defined as `multiboot_info_t`.

This structure contains various flags and fields that describe the system's state at boot time, including:

*   **Memory Map:** A crucial part of `multiboot_info_t` is the memory map, which lists all available and reserved memory regions in the system. This is essential for the frame allocator to correctly identify free physical memory.
*   **Boot Device:** Information about the device from which the kernel was loaded.
*   **Command Line:** Any command-line arguments passed to the kernel.

The `multiboot_info_t` structure is passed to the kernel's entry point (`kmain` in our case) by the bootloader. The `kmain` function receives two arguments:

1.  `multiboot_magic`: A magic number that the bootloader sets to `0x2BADB002` to indicate that it is a Multiboot-compliant bootloader.
2.  `multiboot_info_addr`: The physical address of the `multiboot_info_t` structure.

The kernel then casts `multiboot_info_addr` to a pointer of type `multiboot_info_t *` to access the information.

Here's a simplified representation of how `multiboot_info_t` is passed and used:

```
+-------------------+     +-----------------------+     +-------------------------+
|    Bootloader     | --> |  kmain(magic, addr)   | --> |  multiboot_info_t *mbi  |
| (e.g., GRUB)      |     |                       |     |  = (multiboot_info_t*)addr; |
+-------------------+     +-----------------------+     +-------------------------+
        |                                                           |
        | (Provides physical address of multiboot_info_t)           | (Kernel accesses memory map, etc.)
        V                                                           V
+---------------------------------------------------------------------------------+
|                       Physical Memory (containing multiboot_info_t)             |
+---------------------------------------------------------------------------------+
```

### Paging Explained

Paging is a memory management scheme that allows the operating system to use virtual memory. Virtual memory is a technique that allows a program to use more memory than is physically available in the system. It also provides a layer of abstraction between the program's view of memory (virtual addresses) and the actual physical memory (physical addresses).

The core idea behind paging is to divide both the virtual address space and the physical address space into fixed-size blocks. These blocks are called:

*   **Pages:** Blocks of virtual memory.
*   **Frames (or Page Frames):** Blocks of physical memory.

In our system, both pages and frames will be 4KB in size.

When a program tries to access a memory address, it uses a **virtual address**. The CPU, with the help of a component called the **Memory Management Unit (MMU)**, translates this virtual address into a **physical address**. This translation process is transparent to the running program.

#### How Virtual to Physical Address Translation Works

The translation process involves two main data structures:

1.  **Page Directory:** The first level of translation. It contains entries that point to Page Tables.
2.  **Page Tables:** The second level of translation. Each entry in a Page Table points to a physical memory frame.

Here's a step-by-step breakdown of the translation process:

1.  **Virtual Address Breakdown:** A 32-bit virtual address is divided into three parts:
    *   **Page Directory Index (10 bits):** Used to select an entry in the Page Directory.
    *   **Page Table Index (10 bits):** Used to select an entry in the Page Table.
    *   **Offset (12 bits):** Used to locate the specific byte within the physical frame.

    ```
    31          22 21          12 11           0
    +------------+------------+----------------+
    | Page Dir.  | Page Table |     Offset     |
    |   Index    |   Index    |                |
    +------------+------------+----------------+
    ```

2.  **Page Directory Lookup:**
    *   The CPU takes the Page Directory Index from the virtual address.
    *   It uses this index to look up an entry in the **Page Directory**. The physical address of the Page Directory is stored in a special CPU register called `CR3`.
    *   The entry in the Page Directory contains the physical address of a **Page Table**.

3.  **Page Table Lookup:**
    *   The CPU takes the Page Table Index from the virtual address.
    *   It uses this index to look up an entry in the **Page Table** (whose physical address was obtained from the Page Directory).
    *   The entry in the Page Table contains the physical address of the **physical frame**.

4.  **Physical Address Calculation:**
    *   The CPU takes the physical address of the frame (from the Page Table entry).
    *   It combines this with the **Offset** from the original virtual address.
    *   The result is the final **physical address** that the CPU uses to access memory.

#### Diagram of Paging Translation

```
+-----------------+
| Virtual Address |
+-----------------+
        |
        V
+---------------------------------------------------------------------------------+
| 31-22 (Page Dir. Index) | 21-12 (Page Table Index) | 11-0 (Offset)              |
+---------------------------------------------------------------------------------+
        |                                |                          |
        |                                V                          |
        |                  +---------------------+                  |
        |                  |     Page Table      |                  |
        |                  | (Physical Address)  |                  |
        |                  +---------------------+                  |
        |                            ^                              |
        |                            |                              |
        V                            |                              V
+-----------------+                  |                      +-----------------+
|      CR3        |------------------+                      |     Offset      |
| (Page Dir. Base)|                                          +-----------------+
+-----------------+                                                  |
        |                                                            |
        V                                                            |
+---------------------+
|    Page Directory   |
| (Physical Address)  |
+---------------------+
        |                                                            |
        V                                                            |
+---------------------+
| Page Directory Entry|----------------------------------------------+
| (Physical Address of|                                              |
|    Page Table)      |                                              |
+---------------------+
        |                                                            |
        V                                                            |
+---------------------+
| Page Table Entry    |----------------------------------------------+
| (Physical Address of|                                              |
|    Physical Frame)  |                                              |
+---------------------+
        |                                                            |
        V                                                            |
+---------------------------------------------------------------------------------+
|                                Physical Address                                 |
+---------------------------------------------------------------------------------+
```

#### Page Faults

If, during the translation process, an entry in the Page Directory or Page Table indicates that the page is "not present" (i.e., not currently mapped to a physical frame), a **page fault** occurs. This is an exception that the CPU raises, and the operating system's page fault handler is invoked. The OS can then decide how to handle the page fault, for example, by loading the required page from disk into a free physical frame and updating the page table.

## 4.2. Physical Memory Management

A frame allocator is responsible for managing the physical memory of the system. It keeps track of which physical memory frames are free and which are in use. A physical memory frame is a fixed-size block of physical memory. In our case, a frame will be 4KB, which is a common page size for x86 systems.

To keep track of the state of each frame, we will use a bitmap. A bitmap is a simple and efficient way to represent a set of boolean values. In our case, each bit in the bitmap will correspond to a physical memory frame. If the bit is 1, the frame is in use. If the bit is 0, the frame is free.

Here is a diagram that illustrates the relationship between physical memory and the frame allocator's bitmap:

```
Physical Memory
+------------------+
|   Frame 0 (0x0)  |
+------------------+
|  Frame 1 (0x1000)|
+------------------+
|  Frame 2 (0x2000)|
+------------------+
|      ...         |
+------------------+
| Frame N (0xN000) |
+------------------+

Bitmap
+---+---+---+-----+---+
| 1 | 0 | 1 | ... | 0 |
+---+---+---+-----+---+
  ^   ^   ^       ^
  |   |   |       |
  |   |   |       Frame N is free
  |   |   Frame 2 is used
  |   Frame 1 is free
  Frame 0 is used
```

The frame allocator will provide two main functions:

*   `alloc_frame()`: This function will find the first free frame in the bitmap, mark it as used (by setting the corresponding bit to 1), and return its physical address.
*   `free_frame()`: This function will take a physical address as input, mark the corresponding frame as free (by setting the bit to 0), and make it available for future allocations.

## 4.3. Kernel and User Space Memory Allocation

Once we have a frame allocator, we can build a higher-level memory allocator that can be used by the kernel to allocate memory for data structures and other purposes. We will implement a simple `kmalloc` function that can allocate blocks of memory of a given size.

Later in the course, we will also see how to provide a memory allocation mechanism for user-space programs.

By the end of this chapter, our kernel will have a working paging system and a physical memory allocator, which are essential building blocks for a modern operating system.

## Header Files

### `src/kernel/include/frame_allocator.h`

This header file will define the interface for the physical memory frame allocator.

```c
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
```

### `src/kernel/include/paging.h`

This header file will define the structures for page directories and page tables, as well as the functions for managing paging.

```c
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
```

### `src/kernel/include/kmalloc.h`

This header file will define the interface for the kernel's memory allocator.

```c
#ifndef KMALLOC_H
#define KMALLOC_H

#include "types.h"

void *kmalloc(uint32_t size);
void kfree(void *ptr);

#endif // KMALLOC_H
```