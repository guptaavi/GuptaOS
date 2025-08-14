# Chapter 4: Memory Management

## Introduction

This chapter delves into the crucial aspect of memory management within our operating system. Effective memory management is vital for an OS to efficiently allocate and deallocate memory resources to various processes, protect memory regions, and provide a consistent view of memory to applications.

We will cover the following topics:

*   The Multiboot Specification and its role in initial memory setup.
*   Fundamental memory management concepts: physical vs. virtual memory, paging, and segmentation.
*   Implementation details of our physical frame allocator.
*   Implementation details of our virtual memory (paging) system.
*   Implementation details of our kernel heap (`kmalloc`).

## The Multiboot Specification

Before our kernel can even begin to manage memory, it needs to be loaded into memory by a bootloader. Our OS uses the [Multiboot Specification](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html), a standard that allows a bootloader (like GRUB) to load an operating system kernel. This specification provides a way for the bootloader to pass important information about the system's memory map, available modules, and other configuration details to the kernel.

### Multiboot Header

Our kernel includes a Multiboot header at its very beginning. This header is recognized by the bootloader and signals that the kernel is Multiboot-compliant. The header contains magic numbers, flags, and checksums. You can find its definition in `src/boot.asm` and `src/kernel/include/multiboot.h`.

### Multiboot Information Structure

Upon loading the kernel, the bootloader passes a pointer to a `multiboot_info_t` structure to the kernel's entry point. This structure contains a wealth of information about the system, including:

*   **Memory Map:** A list of available memory regions, indicating their start addresses and lengths. This is crucial for the kernel to understand the physical memory layout and manage it effectively.
*   **Boot Device:** Information about the device from which the kernel was loaded.
*   **Command Line:** Any command-line arguments passed to the kernel.
*   **Modules:** Information about any modules loaded by the bootloader alongside the kernel.

We will explore how our kernel parses this information to set up its initial memory management structures.

## Memory Management Concepts

### Physical vs. Virtual Memory

*   **Physical Memory:** The actual RAM installed in the system. It's addressed directly by the CPU's memory controller.
*   **Virtual Memory:** An abstraction layer provided by the operating system. Each process gets its own virtual address space, which is typically much larger than the available physical memory. The OS maps virtual addresses to physical addresses.

### Paging

Our OS uses paging for virtual memory management. Paging divides both physical and virtual memory into fixed-size blocks:

*   **Pages:** Blocks of virtual memory.
*   **Page Frames:** Blocks of physical memory.

The CPU's Memory Management Unit (MMU) translates virtual addresses to physical addresses using **page tables**. Page tables are hierarchical data structures stored in physical memory.

#### Page Table Structure (Diagram Concept)

*   **Page Directory:** The top-level page table. Each entry points to a Page Table.
*   **Page Table:** Contains entries that point to actual physical page frames.

```mermaid
graph TD
    A[Virtual Address] --> B{MMU}
    B --> C[Page Directory Entry (PDE)]
    C --> D[Page Table Entry (PTE)]
    D --> E[Physical Address]
```

### Segmentation (Briefly)

While x86 architecture supports segmentation, modern operating systems primarily rely on paging for memory management. We will use a flat segmentation model where segments cover the entire address space, effectively disabling segmentation and relying on paging for protection and memory isolation.

## Physical Frame Allocator (`frame_allocator.c` and `frame_allocator.h`)

This component is responsible for managing physical memory. It keeps track of which physical page frames are free and which are in use. When the kernel needs a block of physical memory, it requests it from the frame allocator.

### Data Structures

The core data structure for the physical frame allocator is a **bitmap**:

*   `static uint32_t *frames_bitmap;`: This is a pointer to an array of `uint32_t`. Each bit in this bitmap represents a physical page frame. A set bit (1) indicates that the corresponding page frame is *in use*, while a cleared bit (0) indicates that the page frame is *free*.
*   `static uint32_t num_frames;`: This variable stores the total number of physical page frames available in the system, calculated based on the total detected physical memory.

The `PAGE_SIZE` is defined as `4096` bytes (4KB), which is the standard page size on x86 systems.

### Helper Functions

*   `static void set_frame_bit(uint32_t frame_addr)`:
    *   Marks the page frame corresponding to `frame_addr` as *in use* in the `frames_bitmap`.
    *   It calculates the `frame_num` by dividing `frame_addr` by `PAGE_SIZE`.
    *   Then, it sets the appropriate bit in the `frames_bitmap` array.

*   `static void clear_frame_bit(uint32_t frame_addr)`:
    *   Marks the page frame corresponding to `frame_addr` as *free* in the `frames_bitmap`.
    *   It calculates the `frame_num` and clears the corresponding bit.

*   `static uint8_t test_frame_bit(uint32_t frame_addr)`:
    *   Checks the status of the page frame corresponding to `frame_addr`.
    *   Returns `1` if the frame is *in use*, `0` if it's *free*.

### Functions

*   `void init_frame_allocator(multiboot_info_t *mbi)`:
    *   **Purpose:** Initializes the physical frame allocator. This is one of the first memory-related functions called by the kernel.
    *   **Process:**
        1.  **Determine Total Memory:** It iterates through the Multiboot memory map (`mbi->mmap_addr` and `mbi->mmap_length`) to find the highest physical address available (`max_addr`). This determines the total physical memory and thus the `num_frames`.
        2.  **Allocate Bitmap:** The `frames_bitmap` is placed at a fixed physical address `0x200000` (2MB). This is a temporary placement; later, this bitmap itself will need to be mapped into virtual memory.
        3.  **Mark All Frames Used:** Initially, all bits in the `frames_bitmap` are set to 1 (marked as *used*). This is a safe default.
        4.  **Mark Available RAM as Free:** It iterates through the Multiboot memory map again. For each `Available RAM` region (type `1`), it clears the bits in the `frames_bitmap` corresponding to the physical addresses within that region, marking them as *free*.
        5.  **Mark Bitmap Pages as Used:** The physical pages occupied by the `frames_bitmap` itself are marked as *used* to prevent the allocator from trying to allocate its own data structures.
        6.  **Mark Kernel Pages as Used:** The physical pages occupied by the kernel code and data (assumed to be from `0x100000` to `0x200000`) are also marked as *used*. This prevents the allocator from overwriting the running kernel.

*   `uint32_t alloc_frame()`:
    *   **Purpose:** Allocates a single free physical page frame.
    *   **Process:**
        1.  It iterates through the `frames_bitmap` word by word (`uint32_t`).
        2.  If a `uint32_t` word is not `0xFFFFFFFF` (meaning it contains at least one free bit), it then iterates through each bit within that word.
        3.  When a free bit (0) is found, the corresponding `frame_addr` is calculated.
        4.  The found frame is then marked as *used* using `set_frame_bit`.
        5.  The physical address of the allocated frame is returned.
        6.  If no free frames are found, it returns `0`.

*   `void free_frame(uint32_t addr)`:
    *   **Purpose:** Frees a previously allocated physical page frame.
    *   **Process:**
        1.  It simply calls `clear_frame_bit(addr)` to mark the page frame corresponding to `addr` as *free* in the `frames_bitmap`.

## Virtual Memory (Paging) (`paging.c` and `paging.h`)

This component handles the mapping of virtual addresses to physical addresses using page tables. It provides functions to map, unmap, and manage page directories and page tables.

### Data Structures

*   `PAGE_DIRECTORY_ENTRY`:
    ```c
    typedef struct {
        uint32_t present : 1;
        uint32_t rw : 1;
        uint32_t user : 1;
        uint32_t pwt : 1;
        uint32_t pcd : 1;
        uint32_t accessed : 1;
        uint32_t dirty : 1;
        uint32_t page_size : 1; // 0 for 4KB, 1 for 4MB
        uint32_t global : 1;
        uint32_t available : 3;
        uint32_t frame : 20; // Page table physical address
    } __attribute__((packed)) PAGE_DIRECTORY_ENTRY;
    ```
    This structure represents an entry in the Page Directory. Key fields:
    *   `present`: If set, the page table (or 4MB page) is present in memory.
    *   `rw`: Read/Write permission. 0 for read-only, 1 for read/write.
    *   `user`: User/Supervisor mode access. 0 for supervisor (kernel), 1 for user.
    *   `frame`: The physical address of the page table (for 4KB pages) or the 4MB page itself (for 4MB pages). This is the most significant 20 bits of the physical address.

*   `PAGE_TABLE_ENTRY`:
    ```c
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
    ```
    This structure represents an entry in a Page Table. Key fields are similar to `PAGE_DIRECTORY_ENTRY`:
    *   `present`: If set, the physical page is present in memory.
    *   `rw`: Read/Write permission.
    *   `user`: User/Supervisor mode access.
    *   `frame`: The physical address of the actual 4KB page frame. This is the most significant 20 bits of the physical address.

*   `static PAGE_DIRECTORY_ENTRY page_directory[1024] __attribute__((aligned(4096)));`:
    This is our main Page Directory, an array of 1024 `PAGE_DIRECTORY_ENTRY` structures. It must be 4KB aligned.

*   `static PAGE_TABLE_ENTRY first_page_table[1024] __attribute__((aligned(4096)));`:
    This is the first Page Table, used to map the first 4MB of virtual memory to the first 4MB of physical memory (identity mapping). It must also be 4KB aligned.

### Functions

*   `void init_paging()`:
    *   **Purpose:** Initializes the paging system. This function sets up the initial page directory and page table, and then enables paging.
    *   **Process:**
        1.  **Initialize Page Directory:** All entries in `page_directory` are initially marked as not present, read/write enabled, and kernel-only access.
        2.  **Identity Map First 4MB:** The `first_page_table` is set up to identity map the first 4MB of virtual memory to the first 4MB of physical memory. This means virtual address `X` will map to physical address `X` for addresses within the first 4MB. This is crucial for the kernel to continue executing after paging is enabled, as its code and data are located in this region.
        3.  **Link Page Table to Directory:** The first entry (`page_directory[0]`) of the `page_directory` is configured to point to the `first_page_table`. This makes the identity mapping active for the first 4MB.
        4.  **Load CR3:** The physical address of the `page_directory` is loaded into the `CR3` control register. `CR3` tells the CPU where to find the current page directory.
        5.  **Enable Paging:** The `PG` (Paging Enable) bit in the `CR0` control register is set. This activates the MMU and enables virtual memory translation.

*   `void map_page(uint32_t virtual_address, uint32_t physical_address)`:
    *   **Purpose:** Maps a given `virtual_address` to a `physical_address` in the page tables.
    *   **Process:**
        1.  **Calculate Indices:** The function extracts the Page Directory Index (`pd_index`) and Page Table Index (`pt_index`) from the `virtual_address`.
        2.  **Check Page Directory Entry:** It checks if the `page_directory` entry corresponding to `pd_index` is present. If not, it means no page table exists for this range of virtual addresses.
        3.  **Allocate New Page Table (if needed):** If the page directory entry is not present, a new physical page frame is allocated using `alloc_frame()` from the physical frame allocator. This new frame will hold the new page table. The page directory entry is then updated to point to this new page table, and the new page table is zeroed out.
        4.  **Get Page Table:** The physical address of the relevant page table is retrieved from the `page_directory` entry, and a pointer to it is cast.
        5.  **Map Page:** The `PAGE_TABLE_ENTRY` corresponding to `pt_index` in the `page_table` is updated. Its `present` bit is set, permissions are configured, and its `frame` field is set to the physical address of the page frame to which the `virtual_address` should map.

### Page Table Structure (Diagram Concept)

```mermaid
graph TD
    A[Virtual Address (32-bit)] --> B{Split}
    B -- 10 bits (Directory Index) --> C[Page Directory (1024 entries)]
    B -- 10 bits (Table Index) --> D[Page Table (1024 entries)]
    B -- 12 bits (Offset) --> E[Physical Page Frame (4KB)]

    C -- PDE points to --> D
    D -- PTE points to --> E

    style A fill:#f9f,stroke:#333,stroke-width:2px
    style B fill:#ccf,stroke:#333,stroke-width:2px
    style C fill:#cfc,stroke:#333,stroke-width:2px
    style D fill:#cfc,stroke:#333,stroke-width:2px
    style E fill:#fcc,stroke:#333,stroke-width:2px
```

This diagram illustrates how a 32-bit virtual address is translated into a physical address using a two-level paging scheme:

1.  The most significant 10 bits of the virtual address are used as an index into the **Page Directory**.
2.  The entry in the Page Directory (PDE) points to a **Page Table**.
3.  The next 10 bits of the virtual address are used as an index into the selected Page Table.
4.  The entry in the Page Table (PTE) points to the base physical address of a **Physical Page Frame**.
5.  The least significant 12 bits of the virtual address (the offset) are directly appended to the physical page frame address to form the final **Physical Address**.

## Kernel Heap (`kmalloc.c` and `kmalloc.h`)

The kernel heap provides dynamic memory allocation for the kernel, similar to `malloc` in user-space applications. It allocates smaller chunks of memory from the physical page frames provided by the frame allocator.

### Data Structures

The current implementation of `kmalloc` does not use complex internal data structures beyond what the `frame_allocator` provides. It directly allocates and frees physical page frames.

### Functions

*   `void *kmalloc(uint32_t size)`:
    *   **Purpose:** Allocates a block of `size` bytes from the kernel heap.
    *   **Process:**
        1.  **Calculate Pages:** It calculates the number of physical pages (`num_pages`) required to satisfy the `size` request. Since `kmalloc` currently allocates memory in page-sized chunks, even a small request will result in at least one full page being allocated.
        2.  **Allocate Frames:** It calls `alloc_frame()` repeatedly to allocate the necessary number of physical page frames.
        3.  **Return Address:** The physical address of the first allocated frame is returned as a `void *` pointer.
        4.  **Error Handling:** If `alloc_frame()` returns `0` (indicating no free frames), `kmalloc` also returns `0`.

    *   **Note:** This is a very basic implementation. It allocates memory in page-sized granularity. This means if you request 100 bytes, you still get a full 4KB page. For more efficient memory usage, a more sophisticated `kmalloc` would manage smaller chunks within pages, potentially using techniques like slab allocation or a buddy system.

*   `void kfree(void *ptr)`:
    *   **Purpose:** Frees a previously allocated block of memory pointed to by `ptr`.
    *   **Process:**
        1.  It simply calls `free_frame()` with the physical address of the pointer. This marks the corresponding physical page frame as free in the bitmap.

    *   **Note:** Similar to `kmalloc`, this `kfree` is simplistic. It assumes that `ptr` points to the beginning of a page-aligned block that was previously allocated by `kmalloc`. In a more advanced `kmalloc` implementation, `kfree` would need to handle deallocating smaller chunks and potentially merging free blocks.
