#ifndef INCLUDE_GDT_H_
#define INCLUDE_GDT_H_

#include <stdint.h>
#include <string.h>

#define KERNEL_STACK_TOP   0xC0800000

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

typedef struct {
    uint32_t prev_tss;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__((packed)) tss_t;

extern struct gdt_entry gdt[6];
extern struct gdt_ptr gp;

static inline void tss_flush(void) {
    __asm__ __volatile__(
        "ltr %%ax"
        :
        : "a"(TSS_SEL)
    );
}

void gdt_set_gate(int name, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
void gdt_install();
void gdt_flush();

#endif
