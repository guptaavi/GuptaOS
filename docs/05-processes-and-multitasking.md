# Chapter 6: Processes and Multitasking

Multitasking is the ability of an operating system to run multiple programs at the same time. In this chapter, we will introduce the concept of a process and implement a simple cooperative multitasking system.

## 6.1. Process Control Blocks (PCBs)

A process is an instance of a running program. To manage processes, the kernel needs to keep track of information about each process, such as its registers, memory map, and state (e.g., running, ready, or blocked). This information is stored in a data structure called a Process Control Block (PCB).

We will define a `struct pcb` that will hold the necessary information for each process.

## 6.2. Context Switching

Context switching is the process of saving the state of one process and restoring the state of another. This is what allows the CPU to switch between different processes, creating the illusion that they are running simultaneously.

We will write an assembly function, `context_switch`, that can save the current CPU registers to a PCB and restore the registers from another PCB.

## 6.3. A Simple Scheduler

The scheduler is the part of the kernel that decides which process to run next. There are many different scheduling algorithms, but for this chapter, we will implement a simple round-robin scheduler.

Our scheduler will maintain a list of ready processes. When it is time to switch processes, the scheduler will pick the next process from the list and switch to it.

We will start with cooperative multitasking, where each process is responsible for yielding control of the CPU to the scheduler. Later, we will move to preemptive multitasking, where the kernel can interrupt a process at any time.

By the end of this chapter, we will be able to create multiple processes and watch them run concurrently in our operating system.
