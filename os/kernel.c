#include "io.h"
#include "gdt.h"

extern void kmain() {
    print("hello world\n");

    gdt_flush();
}
