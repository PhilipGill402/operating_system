#ifndef INCLUDE_EXEC_TSS_H_
#define INCLUDE_EXEC_TSS_H_

#include <stdint.h>
#include <string.h>

#include <arch/cpu/gdt.h>

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

extern tss_t tss;

static inline void tss_flush(void) {
    __asm__ __volatile__("ltr %%ax" : : "a"(0x28));
}

void tss_set_kernel_stack(uint32_t kernel_stack_top);
void write_tss(int32_t num, uint16_t ss0, uint32_t esp0);

#endif // !INCLUDE_EXEC_TSS_H_
