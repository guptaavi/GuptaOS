# Chapter 2: The Booting Process

In this chapter, we will dive into the details of how a computer starts up and how we can get our own 32-bit code to run. This process is called booting.

## 2.1. The Boot Sequence (BIOS and GRUB)

When you turn on a computer, it goes through a series of steps to load the operating system. For this course, we will focus on the traditional BIOS boot process. The BIOS initializes hardware and then loads a small program called a bootloader from a bootable device.

A bootloader is responsible for loading the main operating system kernel. For our operating system, we will use **GRUB (GRand Unified Bootloader)**. GRUB handles setting up the hardware, loading our kernel into memory, and then jumping to its entry point.

## 2.2. Creating a Minimal Bootable Kernel

To create a bootable 32-bit kernel, we need to create a few essential files:
1.  A boot assembly file (`src/boot.asm`)
2.  A C kernel file (`src/kmain.c`)
3.  A linker script (`src/linker.ld`)
4.  A GRUB configuration file (`grub.cfg`)

## 2.3. The Boot Assembly File (`boot.asm`)

The `src/boot.asm` file is written in NASM assembly language and serves as the initial entry point for our kernel when it's loaded by GRUB. It sets up the environment for our C code.

```assembly
bits 32  ;nasm directive - 32 bit
        section .text
         ;multiboot spec
         align 4
         dd 0x1BADB002 ;magic
         dd 0x00 ;flags
         dd - (0x1BADB002 + 0x00) ;checksum
         
        global start ;to set symbols from source code as global 
        extern kmain ;kmain is the function in C file
         
        start:
         cli ;clear interrupts-- to diable interrupts
         mov esp, stack_space ;set stack pointer
         call kmain ;calls the main kernel function from c file
         hlt ;halts the CPU
         
        section .bss
        resb 8192 ;8KB memory reserved for the stack
        stack_space:  
```

### 2.3.1. Explanation

*   **`bits 32`**: This NASM directive tells the assembler to generate 32-bit code.
*   **`section .text`**: This is the code section.
*   **Multiboot Header**: This block contains the Multiboot1 header, which GRUB looks for to identify a bootable kernel.
    *   `0x1BADB002`: The magic number that GRUB expects.
    *   `0x00`: Flags indicating no special features are requested.
    *   The checksum ensures the integrity of the header.
*   **`global start`**: Makes the `start` label visible to the linker, serving as our kernel's entry point.
*   **`extern kmain`**: Declares that `kmain` is defined externally (in our C code).
*   **`start:`**: The actual entry point.
    *   `cli`: Clears the interrupt flag, disabling interrupts.
    *   `mov esp, stack_space`: Sets up the stack pointer to the top of our reserved stack space.
    *   `call kmain`: Transfers control to our C kernel's `kmain` function.
    *   `hlt`: Halts the CPU if `kmain` ever returns.
*   **`section .bss`**: This section is for uninitialized data.
*   **`resb 8192`**: Reserves 8KB of memory for the kernel stack.
*   **`stack_space:`**: Marks the top of the stack.

## 2.4. The C Kernel File (`kmain.c`)

The `src/kmain.c` file contains the main logic of our kernel, written in C. It directly interacts with the VGA text mode buffer to display output.

```c
/* kmain.c */

void kmain(void)
{
    const char *string = "Basic Kernel!!";
    char *videomemptr = (char *)0xb8000; // video memory
    unsigned int i = 0;
    unsigned int j = 0;

    // loop to clear the screen - writing the blank character
    // the memory mapped supports 25 lines with 80ascii char with 2bytes of mem each
    while (j < 80 * 25 * 2)
    {
        videomemptr[j] = ' ';      // blank character
        videomemptr[j + 1] = 0x02; // attribute-byte 0 - black background 2 - green font
        j = j + 2;
    }
    j = 0;
    // loop to write the string to the video memory - each character with 0x02 attribute(green)
    while (string[j] != '\0')
    {
        videomemptr[i] = string[j];
        videomemptr[i + 1] = 0x02;
        ++j;
        i = i + 2;
    }
    return;
}
```

### 2.4.1. Explanation

*   **`kmain(void)`**: This is the C entry point called from `boot.asm`.
*   **`videomemptr = (char *)0xb8000`**: This line gets a pointer to the VGA text mode buffer, which is located at physical address `0xB8000`. Writing characters and their attributes to this memory location directly displays them on the screen.
*   **Screen Clearing Loop**: The first `while` loop iterates through the entire screen memory, writing a blank character (`' '`) and a green attribute byte (`0x02`) to clear the screen.
*   **String Printing Loop**: The second `while` loop iterates through the "Basic Kernel!!" string, writing each character and its green attribute byte to the video memory, effectively displaying the message.

## 2.5. The Linker Script (`linker.ld`)

The linker script tells the linker how to organize the different parts of our code and data into the final `kernel.bin` executable file.

```ld
OUTPUT_FORMAT(elf32-i386)
        ENTRY(start)
        SECTIONS
        {
            . = 0x100000;
            .text : { *(.text) }
            .data : { *(.data) }
            .bss  : { *(.bss) }
        }  
```

### 2.5.1. Explanation

*   **`OUTPUT_FORMAT(elf32-i386)`**: Specifies that the output executable should be a 32-bit ELF file for the i386 architecture.
*   **`ENTRY(start)`**: Sets the entry point of our kernel to the `start` symbol, which is defined in `boot.asm`.
*   **`SECTIONS`**: This block defines the memory layout of our kernel.
    *   **`. = 0x100000;`**: This sets the location counter to 1 megabyte (`1MB`). Our kernel will be loaded at this physical memory address, safely above the low memory area used by the BIOS.
    *   **`.text : { *(.text) }`**: Places all code sections (including the Multiboot header from `boot.asm`) into the `.text` section. The linker automatically places the `.multiboot` section first due to its special name and the Multiboot specification.
    *   **`.data : { *(.data) }`**: Places initialized data sections.
    *   **`.bss : { *(.bss) }`**: Places uninitialized data sections.

## 2.6. The Build Process (`Makefile`)

The `Makefile` automates the process of compiling our assembly and C code, linking them, and running the kernel in QEMU.

```makefile
# Makefile

# Compiler and Linker
CC ?= gcc
LD ?= ld

# Assembler
AS = nasm

# Flags
CFLAGS = -m32
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T src/linker.ld

# Source files
C_SOURCES = src/kmain.c
ASM_SOURCES = src/boot.asm

# Object files
C_OBJECTS = ${C_SOURCES:.c=.o}
ASM_OBJECTS = ${ASM_SOURCES:.asm=.o}

# Targets
all: kernel.bin

kernel.bin: ${ASM_OBJECTS} ${C_OBJECTS}
	${LD} ${LDFLAGS} -o kernel.bin ${ASM_OBJECTS} ${C_OBJECTS}

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@

%.o: %.asm
	${AS} ${ASFLAGS} $< -o $@

# Run in QEMU
run: 
	qemu-system-i386 -kernel kernel.bin

# Clean up
clean:
	rm -rf *.o src/*.o kernel.bin
```

### 2.6.1. Explanation

*   **`CC`, `LD`, `AS`**: Define the compiler (`gcc`), linker (`ld`), and assembler (`nasm`) to be used.
*   **`CFLAGS`, `ASFLAGS`, `LDFLAGS`**: Define the flags for 32-bit compilation, assembly, and linking, respectively.
*   **`all: kernel.bin`**: The default target; `make` will build `kernel.bin`.
*   **`kernel.bin: ...`**: Rule to link object files into the final `kernel.bin`.
*   **`%.o: %.c` and `%.o: %.asm`**: Rules to compile C and assembly source files into object files.
*   **`run: ...`**: Rule to launch QEMU with our `kernel.bin`. `qemu-system-i386` is used for 32-bit emulation.
*   **`clean: ...`**: Rule to remove generated build artifacts.

## 2.7. The GRUB Configuration File (`grub.cfg`)

The `grub.cfg` file tells GRUB how to load our kernel.

```
set timeout=0
set default=0

menuentry "guptaOS" {
    multiboot /boot/kernel.bin
    boot
}
```

### 2.7.1. Explanation

*   **`set timeout=0`**: Sets the GRUB menu timeout to 0 seconds, so it boots immediately.
*   **`set default=0`**: Selects the first menu entry by default.
*   **`menuentry "guptaOS"`**: Defines a bootable entry named "guptaOS".
*   **`multiboot /boot/kernel.bin`**: This command tells GRUB to load our `kernel.bin` as a Multiboot1-compliant kernel. The path `/boot/kernel.bin` refers to the location of our kernel within the ISO image.
*   **`boot`**: This command transfers control to the loaded kernel.

## 2.8. Building and Running the Kernel

With all the pieces in place, you can now build and run your minimal 32-bit kernel.

To build the kernel, run `make` in your project's root directory:

```bash
make
```

This will create the `kernel.bin` executable.

To run the kernel in QEMU, execute:

```bash
make run
```

You should see QEMU start, and after a moment, a screen displaying "Basic Kernel!!" in green text. This confirms that your kernel has successfully booted and executed its C code.