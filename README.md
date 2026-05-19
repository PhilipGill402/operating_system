# Custom x86 Operating System

A hobby operating system written in C for 32-bit x86. The project includes a freestanding kernel, a small custom libc, process management, paging, a virtual file system, an initrd-backed filesystem, basic device nodes, ELF program loading, and early user-space support.

This project is built as a learning-focused OS from the ground up, with the goal of understanding how kernels manage memory, processes, filesystems, system calls, and user programs.

---

## Features

### Kernel Core

- 32-bit x86 protected-mode kernel
- GRUB / Multiboot boot support
- Global Descriptor Table setup
- Task State Segment support for switching between kernel and user mode
- VGA text-mode terminal output
- Serial output for debugging
- Programmable interval timer support
- Interrupt and syscall handling

### Memory Management

- Physical frame allocator using a bitmap
- Paging support with kernel and user mappings
- Higher-half kernel layout
- Temporary physical page mappings for copying and inspecting memory
- Kernel heap with `kmalloc`, `kzmalloc`, `kfree`, and `krealloc`
- User heap support through `brk` and `sbrk`

### Process Management

- Process table
- Process states: running, ready, blocked, and terminated
- Per-process page directories
- User stack and heap initialization
- Trapframe-based entry into user mode
- Round-robin scheduler
- `fork`
- `execve`
- `exit`
- `waitpid`
- Parent wakeup after child termination

### ELF Loading

- ELF32 executable validation
- Program header loading
- User process creation from ELF files
- Executable replacement through `execve`

### Filesystem

- Virtual filesystem node abstraction
- File, directory, device, and proc node types
- Initrd filesystem support
- Path resolution with absolute and relative paths
- File descriptors per process
- Basic directory enumeration
- `/dev` support
- `/proc` support

### System Calls

The kernel and libc currently support syscall wrappers for:

- `read`
- `write`
- `open`
- `close`
- `lseek`
- `getdents`
- `getcwd`
- `chdir`
- `getpid`
- `brk`
- `sbrk`
- `fork`
- `execve`
- `waitpid`
- `exit`

### Custom libc

This OS includes a small freestanding C library with support for:

- Character classification through `ctype.h`
- String utilities and a custom `string_t`
- Memory functions such as `memcpy`, `memmove`, `memcmp`, and `memset`
- Dynamic allocation through `malloc`, `free`, and `realloc`
- Basic stdio functions like `printf`, `sprintf`, `snprintf`, `putchar`, `puts`, and `getchar`
- Math utilities such as `sqrt`, `sin`, `cos`, `exp`, `floor`, `ceil`, and `trunc`
- Generic data structures including vectors, queues, and stacks

---

## Project Structure

A typical layout for this project is:

```txt
.
├── kernel/
│   ├── exec/
│   ├── fs/
│   ├── interrupts/
│   ├── memory/
│   └── io/
├── libc/
│   ├── include/
│   └── src/
├── user/
│   └── user programs
├── fs/
│   └── initrd filesystem contents
├── include/
│   └── kernel headers
├── build/
├── isodir/
└── Makefile
```

The exact folder names may vary, but the codebase is organized around a freestanding kernel, a separate libc, user programs, and a generated filesystem image.

---

## Architecture Overview

### Boot Flow

The OS is intended to boot through a Multiboot-compatible bootloader such as GRUB. The bootloader passes a `multiboot_info_t` structure to the kernel, which contains memory information, boot modules, memory maps, and other bootloader-provided data.

The kernel then initializes core architecture pieces such as the GDT, TSS, paging, physical memory manager, heap, interrupts, timer, filesystem, and scheduler.

### Memory Model

The kernel uses a higher-half memory layout starting at `0xC0000000`. Paging maps kernel memory into the higher-half address space while maintaining user-space mappings below the kernel region.

Important constants include:

```c
#define KERNEL_BASE 0xC0000000
#define PAGE_SIZE 4096
```

The memory system includes:

- Physical frame allocation
- Kernel virtual memory allocation
- Page mapping and unmapping
- User page mapping
- Temporary physical mappings
- Kernel heap allocation

### Process Model

Each process tracks its own:

- PID and parent PID
- Process state
- Entry point
- User heap
- User stack
- Kernel stack
- Page directory
- Memory ranges
- Trapframe
- Open file descriptors
- Current working directory

Processes are scheduled through a simple round-robin scheduler. User mode is entered by restoring a saved trapframe and using `iret`.

### Filesystem Model

The filesystem is built around a generic `fs_node_t` abstraction. Each node may represent a file, directory, device, or proc entry. Nodes provide function pointers for operations such as reading, directory enumeration, lookup, creation, and writing.

The initrd filesystem uses a custom node table format and is loaded as an initial RAM disk at boot.

### User Space

User programs are compiled as ELF32 binaries and loaded by the kernel. The libc provides syscall wrappers so user programs can call into the kernel for file operations, process control, heap growth, and basic I/O.

---

## Building

This project expects an `i686-elf` cross compiler.

Required tools usually include:

```txt
i686-elf-gcc
i686-elf-as
i686-elf-ld
i686-elf-ar
grub-mkrescue
xorriso
qemu-system-i386
```

Example build command:

```sh
make
```

To run in QEMU:

```sh
make run
```

To clean build artifacts:

```sh
make clean
```

Exact targets may depend on the current Makefile.

---

## Debugging

Serial output is available through COM1 and is useful for kernel debugging. VGA text output is also supported for basic terminal display.

Typical QEMU serial debugging can be done with something like:

```sh
qemu-system-i386 -cdrom build/os.iso -serial stdio
```

or:

```sh
qemu-system-i386 -cdrom build/os.iso -serial file:serial.log
```

---

## Current Status

This OS currently has early support for:

- Booting into a higher-half kernel
- Paging and heap allocation
- Physical memory allocation
- Basic filesystem and initrd loading
- ELF loading
- User process creation
- Process scheduling
- Syscalls
- Basic libc functionality
- User-space program execution

The project is still experimental and under active development.

---

## Goals

Future development goals include:

- More complete shell support
- More robust filesystem implementation
- Better process cleanup
- Pipes and IPC
- Expanded libc support
- Better debugging tools
- More complete device support
- Improved user-space program loading
- File writing and persistent storage
- More POSIX-like syscall behavior

---

## Notes

This project is primarily educational. It is not intended to be production-ready or POSIX-complete. The goal is to learn how operating systems work internally by implementing core kernel subsystems manually.

---

## License

Add your license here.

For example:

```txt
MIT License
```
