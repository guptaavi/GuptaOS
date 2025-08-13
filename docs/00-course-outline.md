# Course Outline: Building an x86_64 Operating System

This course will guide you through the process of creating a simple, multi-user operating system from scratch for the x86_64 architecture.

## Chapters

1.  **Chapter 1: Introduction to OS Development**
    *   Setting up the development environment (GCC, NASM, QEMU)
    *   Overview of the x86_64 architecture
    *   Introduction to the tools and debugging techniques

2.  **Chapter 2: The Booting Process**
    *   Understanding the boot sequence (BIOS and UEFI)
    *   The role of a bootloader (GRUB)
    *   Creating a minimal bootable kernel

3.  **Chapter 3: "Hello, World" from Bare Metal**
    *   Setting up a minimal C environment
    *   Printing to the console
    *   Compiler and linker basics

4.  **Chapter 4: Interrupts and Exception Handling**
    *   The Interrupt Descriptor Table (IDT)
    *   Handling hardware interrupts (e.g., keyboard)
    *   Handling CPU exceptions (e.g., page faults)

5.  **Chapter 5: Memory Management**
    *   Paging and virtual memory
    *   Physical memory management (frame allocator)
    *   Kernel and user space memory allocation

6.  **Chapter 6: Processes and Multitasking**
    *   Process Control Blocks (PCBs)
    *   Context switching
    *   A simple scheduler

7.  **Chapter 7: Interacting with Devices**
    *   Keyboard driver
    *   Serial communication
    *   Console output

8.  **Chapter 8: Filesystems and Storage**
    *   Introduction to filesystems (e.g., FAT32, ext2)
    *   Implementing a simple virtual filesystem (VFS)
    *   Interacting with a storage device (e.g., a ramdisk)

9.  **Chapter 9: User Mode and System Calls**
    *   Transitioning from kernel mode to user mode
    *   Implementing a system call interface
    *   Running a simple user program

10. **Chapter 10: Multi-user OS**
    *   User accounts and authentication
    *   Permissions and access control
    *   Separating user processes

11. **Chapter 11: Building and Debugging**
    *   Advanced debugging with GDB
    *   Automating the build process
    *   Creating a bootable ISO image

12. **Chapter 12: Final Project**
    *   Putting all the pieces together
    *   Demonstrating the final OS
    *   Ideas for further development
