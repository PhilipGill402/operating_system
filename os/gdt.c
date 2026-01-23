#include "gdt.h"

struct gdt_entry gdt[3];
struct gdt_ptr gp;

void gdt_flush() {
    __asm__ __volatile__ (
        "lgdt %0\n"
        "movw $0x10, %%ax\n"
        "movw %%ax, %%ds\n"
        "movw %%ax, %%es\n"
        "movw %%ax, %%fs\n"
        "movw %%ax, %%gs\n"
        "movw %%ax, %%ss\n"
        "ljmp $0x08, $1f\n"
        "1:\n"
        :
        : "m"(gp)
        : "ax", "memory"
    );
}
