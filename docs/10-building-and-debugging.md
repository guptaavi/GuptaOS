# Chapter 11: Building and Debugging

In this chapter, we will focus on the practical aspects of building and debugging our operating system. We will automate the build process using Make and learn some advanced debugging techniques with GDB.

## 11.1. Advanced Debugging with GDB

We have already mentioned GDB as a tool for debugging our kernel. In this section, we will explore some of its more advanced features:

*   **Connecting to QEMU:** We will learn how to connect GDB to a running QEMU session to debug our kernel live.
*   **Setting breakpoints:** We will see how to set breakpoints at specific lines of code or memory addresses.
*   **Inspecting memory and registers:** We will learn how to use GDB to inspect the contents of memory and the CPU registers.
*   **Creating GDB scripts:** We can automate common debugging tasks by writing GDB scripts.

## 11.2. Automating the Build Process

As our operating system grows, the build process will become more complex. We will use Make to create a `Makefile` that automates the entire build process, from compiling the C and assembly files to linking them into a final kernel executable.

Our `Makefile` will also include targets for:

*   **Cleaning the build directory:** A `clean` target to remove all intermediate files.
*   **Running QEMU:** A `run` target to build the kernel and run it in QEMU.
*   **Debugging with GDB:** A `debug` target to run QEMU in a mode that allows GDB to connect to it.

## 11.3. Creating a Bootable ISO Image

Finally, we will learn how to create a bootable ISO image that contains our kernel and the GRUB bootloader. This ISO image can be burned to a CD or USB stick and used to boot our operating system on a real computer.

By the end of this chapter, we will have a streamlined development workflow that will allow us to build, test, and debug our operating system efficiently.
