#include "idt.h"
#include "io.h"

struct idt_entry idt[256];
struct idt_ptr ip;

struct idt_ptr cur;

void idt_debug_dump_idtr(void) {
    __asm__ __volatile__("sidt %0" : "=m"(cur));
    print("IDTR loaded\n");

    // print cur.base and cur.limit with whatever hex printer you have
    //put_char() 
}

void idt_set_gate(unsigned char num, unsigned long base, unsigned short seg, unsigned char flags) {
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].base_low = base & 0xFFFF;
    idt[num].seg = seg;
    idt[num].ignore = 0;
    idt[num].flags = flags;
}

void idt_create_stubs() {
    unsigned short cs = 0x08;
    unsigned char flags = 0x8E;

    idt_set_gate(0,  (unsigned int)isr0,  cs, flags);
    idt_set_gate(1,  (unsigned int)isr1,  cs, flags);
    idt_set_gate(2,  (unsigned int)isr2,  cs, flags);
    idt_set_gate(3,  (unsigned int)isr3,  cs, flags);
    idt_set_gate(4,  (unsigned int)isr4,  cs, flags);
    idt_set_gate(5,  (unsigned int)isr5,  cs, flags);
    idt_set_gate(6,  (unsigned int)isr6,  cs, flags);
    idt_set_gate(7,  (unsigned int)isr7,  cs, flags);
    idt_set_gate(8,  (unsigned int)isr8,  cs, flags);
    idt_set_gate(9,  (unsigned int)isr9,  cs, flags);
    idt_set_gate(10, (unsigned int)isr10, cs, flags);
    idt_set_gate(11, (unsigned int)isr11, cs, flags);
    idt_set_gate(12, (unsigned int)isr12, cs, flags);
    idt_set_gate(13, (unsigned int)isr13, cs, flags);
    idt_set_gate(14, (unsigned int)isr14, cs, flags);
    idt_set_gate(15, (unsigned int)isr15, cs, flags);
    idt_set_gate(16, (unsigned int)isr16, cs, flags);
    idt_set_gate(17, (unsigned int)isr17, cs, flags);
    idt_set_gate(18, (unsigned int)isr18, cs, flags);
    idt_set_gate(19, (unsigned int)isr19, cs, flags);
    idt_set_gate(20, (unsigned int)isr20, cs, flags);
    idt_set_gate(21, (unsigned int)isr21, cs, flags);
    idt_set_gate(22, (unsigned int)isr22, cs, flags);
    idt_set_gate(23, (unsigned int)isr23, cs, flags);
    idt_set_gate(24, (unsigned int)isr24, cs, flags);
    idt_set_gate(25, (unsigned int)isr25, cs, flags);
    idt_set_gate(26, (unsigned int)isr26, cs, flags);
    idt_set_gate(27, (unsigned int)isr27, cs, flags);
    idt_set_gate(28, (unsigned int)isr28, cs, flags);
    idt_set_gate(29, (unsigned int)isr29, cs, flags);
    idt_set_gate(30, (unsigned int)isr30, cs, flags);
    idt_set_gate(31, (unsigned int)isr31, cs, flags);
}

void idt_install() {
    ip.limit = sizeof(struct idt_entry) * 256 - 1;
    ip.base = (unsigned int)idt;

    //zero out the idt
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i,0,0,0);
    }

    //add all entries here 
    idt_create_stubs(); 
    
    unsigned int addr = ((unsigned int)idt[3].base_hi << 16) | idt[3].base_low;
    printi(idt[3].base_hi);
    print("\n");
    printi(idt[3].base_low);
    print("\n");
    //printi(idt[3].seg);
    //print("\n");
    //printi(idt[3].flags);
    //print("\n");

    idt_load();
    idt_debug_dump_idtr(); 
    //__asm__ __volatile__ ("sti");
}

void idt_load() {
    __asm__ __volatile__ ("lidt %0" : : "m"(ip) : "memory");
}

void isr_handler(regs_t* reg) {
    print("EXCEPTION: ");
    
    //infinite wait
    while (1) {
        __asm__ __volatile__("hlt");
    }
}
