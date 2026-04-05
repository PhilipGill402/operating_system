#include "syscalls.h"

void sys_exit() {
    uint32_t size;
    uint8_t* elf = elf_from_file(current_process->file, &size);
    Elf32_Ehdr* header = (Elf32_Ehdr*)elf;
    
    // cleans up all the mapped virtual memory
    for (uint32_t offset = header->e_phoff; offset < header->e_phoff + header->e_phentsize * header->e_phnum; offset += header->e_phentsize) {
        Elf32_Phdr* prog_header = (Elf32_Phdr*)((uint32_t)elf + offset);
        if (prog_header->p_type != PT_LOAD) {
            continue;
        }

        uint32_t start = prog_header->p_vaddr & ~(PAGE_SIZE - 1);
        uint32_t end = (prog_header->p_vaddr + prog_header->p_memsz + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

        for (uint32_t addr = start; addr < end; addr += PAGE_SIZE) {
            uint32_t frame = unmap_page(addr);
            if (frame) {
                pmm_free_frame(frame);
            }
        }
    }

    // cleans up the stack memory
    for (uint32_t addr = current_process->user_stack_bottom; addr < current_process->user_stack_top; addr += PAGE_SIZE) {
        uint32_t frame = unmap_page(addr);
        if (frame) {
            pmm_free_frame(frame);
        }
    }

    current_process = NULL;
    
    __asm__ __volatile__("sti");

    tty();
}

void syscall_handler(regs_t* reg) {
    if (reg->eax == SYS_EXIT) {
        sys_exit();
    }
}
