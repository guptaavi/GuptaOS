# Project Overview

This project is an educational endeavor to build a simple x86 (32-bit) operating system from scratch. It covers fundamental OS concepts such as booting, memory management, and console interaction. The project is structured as a course, with detailed documentation in Markdown files.

## Main Technologies

*   **Assembly (NASM):** For low-level boot code (32-bit).
*   **C (GCC):** For the main kernel logic (32-bit).
*   **GRUB:** As the bootloader.
*   **QEMU:** For emulation and testing.
*   **Make:** For build automation.

## Building and Running

The `Makefile` orchestrates the build process.

*   **Clean build artifacts:**
    ```bash
    make clean
    ```
*   **Build the kernel:**
    ```bash
    make
    ```
    This command compiles the assembly and C source files, and links them into `kernel.bin`.
*   **Run the OS in QEMU:**
    ```bash
    make run
    ```
    This command launches QEMU with `kernel.bin` directly.



## Development Conventions

*   **Kernel Entry Point:** The kernel starts execution at the `start` label in `src/boot.asm`, which then calls the `kmain` function in `src/kmain.c`.
*   **Kernel Load Address:** The kernel is loaded at `1MB` in physical memory, as defined in `src/linker.ld`.
*   **VGA Text Mode:** Early console output is achieved by directly writing to the VGA text-mode buffer at `0xB8000`. The `kmain.c` clears the screen and prints "Basic Kernel!!" with green font.
*   **Multiboot:** The kernel uses the Multiboot1 specification.
*   **Documentation:** Detailed explanations, code snippets, and troubleshooting guides are provided in Markdown files within the `docs/` directory, structured as chapters.
