#include "exec/tss.h"

tss_t tss;

void tss_set_kernel_stack(uint32_t kernel_stack_top) {
    tss.esp0 = kernel_stack_top;
}

void write_tss(int32_t num, uint16_t ss0, uint32_t esp0) {
    uint32_t base = (uint32_t)&tss;
    uint32_t limit = sizeof(tss_t);

    gdt_set_gate(num, base, base + limit, 0xE9, 0x00);

    memset(&tss, 0, sizeof(tss_t));
    tss.ss0 = ss0;
    tss.esp0 = esp0;

    tss.cs = 0x0b;
    tss.ss = 0x13;
    tss.ds = 0x13;
    tss.es = 0x13;
    tss.fs = 0x13;
    tss.gs = 0x13;

    tss.iomap_base = sizeof(tss_t);
}
