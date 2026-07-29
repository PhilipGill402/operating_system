#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <stack.h>
#include <log.h>

#include "arch/boot.h"
#include "io/io.h"
#include "io/vga.h"
#include "io/serial.h"
#include "io/framebuffer.h"
#include "gdt.h"
#include "multiboot.h"
#include "memory/physical_allocator.h"
#include "memory/paging.h"
#include "memory/heap.h"
#include "interrupts/idt.h"
#include "interrupts/irq.h"
#include "interrupts/pit.h"
#include "interrupts/mouse.h"
#include "interrupts/keyboard.h"
#include "interrupts/events.h"
#include "fs/fs.h"
#include "exec/scheduler.h"
#include "exec/elf.h"
#include "hardware.h"
#include "memory_mapping.h"

multiboot_info_t* mbi;

__attribute__((noreturn)) 
void kernel_finish_init(void) {
    gdt_install();
    idt_install();
    
    pic_remap(0x20, 0x28);
    irq_init_handlers();
    
    pic_clear_mask(0);
    pic_clear_mask(1);
    pit_init(100);
     
    init_heap();
    
    fs_init(mbi);

    // GRUB multiboot supports use of the framebuffer
    if (mbi->flags & (1 << 12)) {
        framebuffer_init(mbi);
    }

    mouse_init();
    events_init();

    scheduler_init();
    
    __asm__ __volatile__("sti");
    fs_node_t* tty_elf = resolve_path("/bin/tty", fs_root);
    if (!tty_elf) {
        log_error("Couldn't load init file\n"); 
    } else {
        elf_execute(tty_elf);
        schedule_and_enter();
    }

    for (;;) {}
}

void kernel_main(uint32_t mbi_phys) {
    arch_i686_kernel_early_init(mbi_phys, kernel_finish_init);
}
