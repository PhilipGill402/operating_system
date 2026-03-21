#ifndef INCLUDE_IDT_H_
#define INCLUDE_IDT_H_

#include <stdio.h>

extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

struct idt_entry {
    unsigned short base_low;
    unsigned short seg;
    unsigned char ignore;
    unsigned char flags;
    unsigned short base_hi;
} __attribute__((packed));

struct idt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

typedef struct regs {
    unsigned int ds;
    unsigned int edi;
    unsigned int esi;
    unsigned int ebp;
    unsigned int esp;
    unsigned int ebx;
    unsigned int edx;
    unsigned int ecx;
    unsigned int eax;
    unsigned int int_no;
    unsigned int err_code;
    unsigned int eip;
    unsigned int cs;
    unsigned int eflags;
    unsigned int useresp;
    unsigned int ss;
} regs_t;

extern struct idt_entry idt[256];
extern struct idt_ptr ip;

void idt_set_gate(unsigned char num, unsigned long base, unsigned short seg, unsigned char flags);
void idt_create_stubs();
void idt_install();
void idt_load();
void isr_handler(regs_t* reg);

#endif // !INCLUDE_IDT_H_
