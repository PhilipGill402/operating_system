#ifndef INCLUDE_GDT_H_
#define INCLUDE_GDT_H_

#include <stdint.h>
#include <string.h>
#include "exec/tss.h"
#include "memory_mapping.h"

#define PAGE_SIZE 4096

#define KERNEL_CS 0x08
#define KERNEL_DS 0x10
#define USER_CS   0x18
#define USER_DS   0x20
#define TSS_SEL   0x28

#define GDT_ACCESS_CODE_PL0 0x9A
#define GDT_ACCESS_DATA_PL0 0x92
#define GDT_ACCESS_CODE_PL3 0xFA
#define GDT_ACCESS_DATA_PL3 0xF2
#define GDT_ACCESS_TSS      0x89
#define GDT_FLAGS_4K_32BIT  0xCF
#define GDT_FLAGS_TSS       0x40

struct gdt_entry {
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __attribute__((packed));

struct gdt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

extern struct gdt_entry gdt[6];
extern struct gdt_ptr gp;

void gdt_set_gate(int name, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
void gdt_install();
void gdt_flush();

#endif
