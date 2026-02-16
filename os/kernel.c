#include "io.h"
#include "gdt.h"
#include "idt.h"

void kmain(void) {
    
    print("Hello world!\n");    


    gdt_install();
    idt_install();

     
}
