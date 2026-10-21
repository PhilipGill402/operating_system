#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <log.h>

#include <arch/boot/boot.h>
#include <arch/asm/helpers.h>
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
    if (!init_heap())
        panic("failed to initialize heap\n");
    log_debug("heap initialized\n");

    input_buffer_alloc();
    log_debug("input buffer initialized\n");
    
    uintptr_t initrd_addr = 0;
    size_t initrd_size = 0;
    for (uint8_t i = 0; i < info->region_count; ++i) {
        if (info->regions[i].type == MEM_FILESYSTEM) {
            initrd_addr = info->regions[i].mem_addr;
            initrd_size = info->regions[i].mem_size;
        }
    }

    fs_init(initrd_addr, initrd_size);
    log_debug("file system initialized\n");

    scheduler_init();
    log_debug("scheduler initialized\n");

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
