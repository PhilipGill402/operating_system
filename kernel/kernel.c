#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <stack.h>
#include <log.h>

#include <arch/boot/boot.h>
#include <arch/cpu/gdt.h>
#include <arch/asm/helpers.h>
#include <arch/memory/physical_allocator.h>
#include <arch/memory/paging.h>

#include "io/io.h"
#include "io/vga.h"
#include "io/serial.h"
#include "io/framebuffer.h"
#include "multiboot.h"
#include "memory/heap.h"
#include "interrupts/mouse.h"
#include "interrupts/keyboard.h"
#include "interrupts/events.h"
#include "fs/fs.h"
#include "exec/scheduler.h"
#include "exec/elf.h"
#include "hardware.h"
#include "memory_mapping.h"

multiboot_info_t* mbi;

__attribute__(())
void kernel_finish_init() {
    arch_kernel_init(); 
     
    init_heap();
    
    fs_init(mbi);

    // GRUB multiboot supports use of the framebuffer
    if (mbi->flags & (1 << 12)) {
        framebuffer_init(mbi);
    }

    mouse_init();
    events_init();

    scheduler_init();
    
    arch_enable_interrupts();

    fs_node_t* tty_elf = resolve_path("/bin/tty", fs_root);
    if (!tty_elf) {
        log_error("Couldn't load init file\n"); 
    } else {
        elf_execute(tty_elf);
        schedule_and_enter();
    }

    arch_halt();
}

void kernel_main(uint32_t mbi_phys) {
    arch_kernel_early_init(mbi_phys);
    arch_kernel_finish_init();
}
