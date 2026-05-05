#ifndef INCLUDE_EXEC_ELF_H_
#define INCLUDE_EXEC_ELF_H_

#include <stdint.h>
#include <string.h>

#include "fs/fs.h"
#include "memory/heap.h"
#include "memory/paging.h"
#include "memory/physical_allocator.h"
#include "exec/process.h"
#include "exec/scheduler.h"

#define PAGE_SIZE 4096

#define STACK_TOP 0x00800000

#define EI_NIDENT 16

/* e_ident offsets */
#define EI_MAG0     0
#define EI_MAG1     1
#define EI_MAG2     2
#define EI_MAG3     3
#define EI_CLASS    4
#define EI_DATA     5
#define EI_VERSION  6

/* Magic bytes */
#define ELFMAG0 0x7F
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

/* ELF class */
#define ELFCLASS32 1

/* Endianness */
#define ELFDATA2LSB 1

/* File types */
#define ET_NONE 0
#define ET_REL  1
#define ET_EXEC 2

/* Machine types */
#define EM_386 3

/* Program header types */
#define PT_NULL 0
#define PT_LOAD 1

/* Program header flags */
#define PF_X 0x1
#define PF_W 0x2
#define PF_R 0x4

typedef struct {
    unsigned char e_ident[EI_NIDENT];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} __attribute__((packed)) Elf32_Ehdr;

typedef struct {
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
} __attribute__((packed)) Elf32_Phdr;

uint8_t* elf_from_file(fs_node_t* elf, uint32_t* size);
uint8_t elf_load(const uint8_t* elf, uint32_t size, process_t* process);
uint8_t elf_validate(const uint8_t* elf, uint32_t size);
void elf_print_info(const uint8_t* elf);
void elf_load_segments(uint8_t* elf, size_t size);
void elf_execute(fs_node_t* elf);
process_t* process_create_from_elf(fs_node_t* elf);
uint32_t process_exec_from_elf(process_t* process, fs_node_t* elf);

#endif // !INCLUDE_EXEC_ELF_H_
