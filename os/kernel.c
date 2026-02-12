#include "io.h"
#include "gdt.h"
#include "idt.h"

extern void kmain() {
    
    __asm__ __volatile__("cli");

    print("hello world\n");
    print("whats up\n");

    //gdt_install();
    //idt_install();

    
    /*
    volatile int a = 1;
    volatile int b = 0;
    volatile int c = a / b;
    (void)c;
    */
}
