# Makefile

# Cross-compiler
# If you built a cross-compiler, uncomment the following line:
# CC = x86_64-elf-gcc
# LD = x86_64-elf-ld
# Otherwise, we will use the host's compiler
CC ?= gcc
LD ?= ld

# Assembler
AS = nasm

# Flags
CFLAGS = -fno-stack-protector -m32 -Isrc/kernel/include
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T src/linker.ld

# Source files
C_SOURCES = src/kernel/main/kmain.c \
            src/kernel/idt/idt.c \
            src/kernel/io/io.c \
            src/kernel/mem/paging.c \
            src/kernel/mem/frame_allocator.c \
            src/kernel/mem/kmalloc.c \
            src/kernel/utils/stack_chk_fail.c
ASM_SOURCES = src/boot.asm

# Object files
C_OBJECTS = $(patsubst %.c, %.o, $(C_SOURCES))
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
	qemu-system-i386 -M isapc -m 64M -kernel kernel.bin -serial file:qemu_output.log -nographic

# Clean up
clean:
	rm -rf *.o src/*.o src/kernel/idt/*.o src/kernel/io/*.o src/kernel/main/*.o src/kernel/utils/*.o kernel.bin
