# Chapter 10: Multi-user OS

In this chapter, we will extend our operating system to support multiple users. This will involve adding concepts like user accounts, authentication, and permissions.

## 10.1. User Accounts and Authentication

To support multiple users, we first need a way to distinguish between them. We will create a simple user account system that stores a username and a hashed password for each user.

We will also implement a login prompt that asks the user for their credentials before giving them access to the system.

## 10.2. Permissions and Access Control

A multi-user operating system needs to be able to control which users can access which resources. We will implement a simple permission system based on the traditional Unix model of users, groups, and permissions (read, write, execute).

Each file and directory in our filesystem will have an owner, a group, and a set of permissions. The kernel will enforce these permissions, preventing unauthorized users from accessing files that they don't have permission to.

## 10.3. Separating User Processes

In a multi-user system, it is essential to isolate the processes of different users from each other. We will extend our process management system to associate each process with a specific user.

The kernel will ensure that a process running on behalf of one user cannot access the memory or resources of a process running on behalf of another user.

By the end of this chapter, our operating system will be a true multi-user system, with support for user accounts, authentication, and access control.
