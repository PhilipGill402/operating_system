#include "io.h"
#include "gdt.h"
#include "idt.h"

void kmain(void) {
    
    print("Hello world!\n");    


    gdt_install();
    idt_install();

    
    volatile int a = 1;
    volatile int b = 0;
    volatile int c = a / b;
    (void)c;
}
