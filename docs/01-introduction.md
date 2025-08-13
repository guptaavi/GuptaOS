# Chapter 1: Introduction to OS Development

Welcome to the first chapter of our journey into operating system development. In this chapter, we will set up our development environment and get a basic understanding of the tools we will be using throughout this course.

## 1.1. Development Environment

To build our operating system, we will need a few key tools:

*   **A Linux environment:** A Linux-based OS (like Ubuntu or Fedora) is recommended. You can use a virtual machine if you are on Windows or macOS.
*   **NASM:** The Netwide Assembler, which we will use to write our assembly code.
*   **GCC:** The GNU Compiler Collection, which we will use to compile our C code.
*   **QEMU:** A machine emulator and virtualizer that we will use to test our operating system without needing to reboot our computer.
*   **Make:** A build automation tool to help us automate the compilation and linking process.

### 1.1.1. Installation

On a Debian-based Linux distribution (like Ubuntu), you can install these tools with the following command:

```bash
sudo apt-get update
sudo apt-get install build-essential nasm qemu-system-x86
```

This command installs `build-essential` (which includes GCC and Make), `nasm`, and `qemu-system-x86`.

## 1.2. Overview of the x86_64 Architecture

Our target architecture is x86_64, the 64-bit version of the x86 architecture. Here are a few key concepts:

*   **Registers:** The CPU has a set of general-purpose registers (RAX, RBX, RCX, etc.), a program counter (RIP), and a stack pointer (RSP).
*   **Operating Modes:** The x86_64 architecture has several operating modes, but we will be primarily concerned with **long mode**, which is the native 64-bit mode.
*   **Memory Model:** Memory is byte-addressable and is managed by the Memory Management Unit (MMU).

## 1.3. Tools and Debugging

*   **Compiler (GCC):** We will use GCC to compile our C code into object files. We will use specific flags to ensure that the code is suitable for a freestanding environment (i.e., an operating system).
*   **Assembler (NASM):** We will use NASM to write the initial boot code that sets up the CPU and hands over control to our C kernel.
*   **Linker (LD):** The linker is responsible for combining our object files into a single executable kernel file. We will use a custom linker script to control the memory layout of our kernel.
*   **Emulator (QEMU):** QEMU will be our primary tool for testing the OS. We can boot our kernel in QEMU and interact with it as if it were running on a real machine.
*   **Debugger (GDB):** We can connect GDB to QEMU to debug our kernel, set breakpoints, and inspect the state of the CPU and memory.

## 1.4. Setting up a Cross-Compiler

While you can use your system's native GCC compiler for the initial stages, it's a good practice to build a cross-compiler. A cross-compiler is a compiler that runs on your host system but generates code for a different target system. In our case, the target is `x86_64-elf`, which means a 64-bit x86 system with no specific operating system (a "bare metal" environment).

### 1.4.1. Why a Cross-Compiler?

Using your host's compiler can sometimes lead to subtle issues because it assumes the presence of a standard C library (libc) and other OS-specific features. A cross-compiler built for a freestanding environment ensures that we don't accidentally link against the host's libraries and gives us full control over the toolchain.

### 1.4.2. Building a Cross-Compiler

To simplify the process of building a cross-compiler, a shell script named `build-cross-compiler.sh` is provided in the root of this project.

To build the cross-compiler, simply run the script from your terminal:

```bash
./build-cross-compiler.sh
```

The script will download the necessary source code for Binutils and GCC, build them, and install the cross-compiler to `$HOME/opt/cross`. It will also prompt you to install the required dependencies.

After the script finishes, make sure to add the cross-compiler to your `PATH` by adding the following line to your shell's startup script (e.g., `.bashrc` or `.zshrc`):

```bash
export PATH="$HOME/opt/cross/bin:$PATH"
```

When you run `x86_64-elf-gcc`, you will be using your new cross-compiler.

In the next chapter, we will start the practical work by creating a bootable kernel that we can run in QEMU.
