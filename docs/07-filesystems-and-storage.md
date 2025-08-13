# Chapter 8: Filesystems and Storage

In this chapter, we will add support for storing and retrieving data. We will start by creating a simple in-memory filesystem (a ramdisk) and then move on to a more general-purpose virtual filesystem (VFS).

## 8.1. Introduction to Filesystems

A filesystem is a method for storing and organizing files on a storage device. There are many different types of filesystems, each with its own advantages and disadvantages. Some common examples include:

*   **FAT32 (File Allocation Table):** A simple filesystem that is widely supported.
*   **ext2/ext3/ext4 (Extended File System):** The standard filesystem for Linux.
*   **NTFS (New Technology File System):** The standard filesystem for Windows.

## 8.2. Implementing a Simple Virtual Filesystem (VFS)

A Virtual Filesystem (VFS) is an abstraction layer that provides a common interface for different filesystems. This allows the kernel to work with different filesystems in a uniform way.

We will design a VFS that defines a set of common file operations, such as `open`, `read`, `write`, and `close`. We will also define data structures to represent files, directories, and mount points.

## 8.3. Interacting with a Storage Device (Ramdisk)

To test our VFS, we will create a simple ramdisk. A ramdisk is a filesystem that is stored in memory instead of on a physical disk. This is a good way to get started with filesystems without having to deal with the complexities of a real hardware storage device.

We will create a ramdisk that is populated with a few files at compile time. We will then mount this ramdisk on our VFS and use our file operations to read the files from it.

By the end of this chapter, our operating system will have a working VFS and a simple ramdisk, which will be the foundation for all future file I/O.
