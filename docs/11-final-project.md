# Chapter 12: Final Project

Congratulations on reaching the final chapter of this course! In this chapter, you will put all the pieces together to create a final, working prototype of your operating system.

## 12.1. Putting It All Together

The goal of the final project is to integrate all the concepts and components that we have developed throughout this course into a single, coherent operating system. This will involve:

*   **Creating a final build system:** You will need to create a `Makefile` that can build your entire operating system, including the kernel, user programs, and a bootable ISO image.
*   **Writing a simple shell:** You will write a simple shell program that can run in user mode and interact with the kernel through system calls. The shell should be able to launch other programs and perform basic file operations.
*   **Demonstrating the final OS:** You will need to prepare a demonstration of your operating system that showcases all of its features, including multitasking, console I/O, file I/O, and multi-user support.

## 12.2. Demonstrating the Final OS

Your final demonstration should show that your operating system meets all of the minimum requirements set out at the beginning of the course:

*   **Booting:** Your OS should be able to boot from an ISO image in QEMU.
*   **Multitasking:** You should be able to run multiple programs concurrently.
*   **Console interaction:** You should be able to interact with your OS through the console.
*   **Storage devices:** You should be able to read and write files from a ramdisk.
*   **Multi-user:** You should be able to log in as different users and demonstrate that their processes are isolated from each other.

## 12.3. Ideas for Further Development

This course has only scratched the surface of what is possible in operating system development. Here are some ideas for how you could extend your OS further:

*   **Add support for more hardware:** You could write drivers for other devices, such as a mouse, a network card, or a real hard drive.
*   **Implement a more advanced scheduler:** You could replace the simple round-robin scheduler with a more sophisticated algorithm, such as a priority-based scheduler.
*   **Port your OS to a different architecture:** You could try porting your OS to a different architecture, such as ARM or RISC-V.
*   **Add a graphical user interface (GUI):** This is a major undertaking, but it would be a great way to learn about graphics programming and windowing systems.

Good luck with your final project!
