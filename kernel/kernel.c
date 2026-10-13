#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <stack.h>
#include <log.h>

#include <arch/boot/boot.h>
#include <arch/asm/helpers.h>
#include <arch/memory/physical_allocator.h>
#include <arch/memory/paging.h>
#include <arch/cpu/cpu_info.h>

#include "kernel.h"
#include "io/io.h"
#include "io/vga.h"
#include "io/framebuffer.h"
#include "io/input_buffer.h"
#include "multiboot.h"
#include "memory/heap.h"
#include "fs/fs.h"
#include "exec/scheduler.h"
#include "exec/elf.h"
#include "memory_mapping.h"

void kernel_main(const kernel_boot_info_t* info) {
    input_buffer_alloc();

    init_heap();
    
    
    //fs_init((multiboot_info_t*)info->boot_info);

    /*
    // GRUB multiboot supports use of the framebuffer
    if (mbi->flags & (1 << 12)) {
        framebuffer_init(mbi);
    }
    */

    //mouse_init();

    scheduler_init();
    
    arch_enable_interrupts();

    fs_node_t* tty_elf = resolve_path("/bin/tty", fs_root);
    if (!tty_elf) {
        log_error("Couldn't load init file\n"); 
    } else {
        log_debug("loading init file\n"); 
        elf_execute(tty_elf);
        schedule_and_enter();
    }
    
    for (;;) {
        arch_halt();
    }
}
