# Chapter 9: User Mode and System Calls

So far, all of our code has been running in kernel mode, with full access to all of the hardware. In this chapter, we will take a major step towards building a modern operating system by introducing the concept of user mode and creating a system call interface to allow user programs to request services from the kernel.

## 9.1. Transitioning from Kernel Mode to User Mode

User mode is a restricted mode of operation in which the CPU has limited access to hardware and memory. This is a crucial security feature that prevents user programs from interfering with the kernel or other programs.

We will learn how to:

*   **Set up user-space segments:** We will need to create new segments in the Global Descriptor Table (GDT) for user-mode code and data.
*   **Prepare a user-space stack:** Each user process will need its own stack.
*   **Jump to user mode:** We will use the `iret` instruction to transition from kernel mode to user mode and start executing a user program.

## 9.2. Implementing a System Call Interface

User programs need a way to request services from the kernel, such as reading a file or allocating memory. This is done through system calls.

A system call is a special instruction that transfers control from user mode to kernel mode and invokes a specific kernel function. We will implement a simple system call interface using the `syscall` and `sysret` instructions.

We will create a system call table that maps system call numbers to their corresponding handler functions in the kernel.

## 9.3. Running a Simple User Program

To test our user mode and system call implementation, we will write a simple user program that prints a message to the console using a system call. We will load this program into memory, set up a user-space environment for it, and then run it.

By the end of this chapter, our operating system will be able to run user programs in a protected environment, which is a major milestone in the development of any modern OS.
