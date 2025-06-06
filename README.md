# Google-Drive-Management-System in C++

A console-based File Management System built in C++ that mimics real-world file handling, version control, user roles, and access control. It supports folder hierarchy, file creation/editing, version tracking, access rights, AVL tree indexing, heap-based file priority, and user management.

## Features

- **User System with Roles**: Admin, Editor, Viewer roles with login/logout and registration.
- **Folder Navigation**: Create subfolders, navigate hierarchy, and manage directory structure.
- **File Management**: Create, update, display, delete, and restore files.
- **Version Control**: Each file maintains a history of versions with timestamps.
- **Recent Files Queue**: Tracks most recently accessed files.
- **Recycle Bin**: Stack-based system to recover deleted files.
- **AVL Tree**: Index and search files by name efficiently.
- **File Sharing**: Share files between users with graph-based user connections.
- **Access Control**: Grant and check file permissions (`read`, `write`, `execute`).
- **Heap-based Priority Queue**: Manage files by custom priority (name-based).

## Technologies

- **C++**
- **OOP Concepts** (Classes, Inheritance, Encapsulation)
- **Data Structures**:
  - Linked List (Version history)
  - Stack (Recycle Bin)
  - Queue (Recent files)
  - AVL Tree (Efficient file search)
  - Heap (File priority)
  - Graph (User relationships)

## Project Structure

```bash
.
├── Main.cpp        # Entire application code
├── README.md       # This file
