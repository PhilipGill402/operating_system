#include <arch/cpu/gdt.h>

struct gdt_entry gdt[6];
struct gdt_ptr gp;

//TODO: add gdt initialization before flushing it
void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran) {
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F);

    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access = access;
}

void gdt_install() {
    // zero out tss and fill it with useful data
    memset(&tss, 0, sizeof(tss_t));
    tss.ss0 = KERNEL_DS;
    tss.esp0 = KERNEL_STACK_TOP;
    tss.iomap_base = sizeof(tss_t);

    gp.limit = sizeof(gdt) - 1;
    gp.base = (unsigned int) &gdt;
    
    //NULL
    gdt_set_gate(0,0,0,0,0);

    //Kernel Code segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, GDT_ACCESS_CODE_PL0, GDT_FLAGS_4K_32BIT);

    //Kernel Data segment
    gdt_set_gate(2, 0, 0xFFFFFFFF, GDT_ACCESS_DATA_PL0, GDT_FLAGS_4K_32BIT);
    
    //User Code segment
    gdt_set_gate(3, 0, 0xFFFFFFFF, GDT_ACCESS_CODE_PL3, GDT_FLAGS_4K_32BIT); 

    //User Data segment
    gdt_set_gate(4, 0, 0xFFFFFFFF, GDT_ACCESS_DATA_PL3, GDT_FLAGS_4K_32BIT);

    //TSS segment
    gdt_set_gate(5, (uint32_t)&tss, sizeof(tss)-1, GDT_ACCESS_TSS, GDT_FLAGS_TSS);

    gdt_flush();
    tss_flush();
}

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


