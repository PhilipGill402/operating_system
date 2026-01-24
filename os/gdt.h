#ifndef OS_GDT_H_
#define OS_GDT_H_

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

extern struct gdt_entry gdt[3];
extern struct gdt_ptr gp;

void gdt_set_gate(int name, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
void gdt_install();
void gdt_flush();


#endif
