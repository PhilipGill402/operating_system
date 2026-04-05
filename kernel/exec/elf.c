#include "exec/elf.h"

process_t* current_process;

uint8_t* elf_from_file(fs_node_t* elf, uint32_t* size) {
    uint8_t* buffer = kmalloc(elf->size);
    fs_read(elf, 0, elf->size, buffer);
    *size = elf->size;

    return buffer;
}

uint8_t elf_validate(const uint8_t* elf, uint32_t size) {
    Elf32_Ehdr* header = (Elf32_Ehdr*)elf; 

    if (size < sizeof(Elf32_Ehdr)) {
        printf("Elf not large enough for ELF header\n"); 
        return 0;
    }

    if (!elf) {
        printf("elf data is NULL\n");
        return 0;
    }

    if (header->e_ident[0] != ELFMAG0) {
        printf("magic validation failed\n"); 
        return 0;
    } else if (header->e_ident[1] != ELFMAG1) {
        printf("magic validation failed\n"); 
        return 0;
    } else if (header->e_ident[2] != ELFMAG2) {
        printf("magic validation failed\n"); 
        return 0;
    }  else if (header->e_ident[3] != ELFMAG3) {
        printf("magic validation failed\n"); 
        return 0;
    }

    if (header->e_ident[4] != ELFCLASS32) {
        printf("not 32 bit ELF\n");
        return 0;
    }

    if (header->e_ident[5] != ELFDATA2LSB) {
        printf("not little endian\n");
        return 0;
    }

    if (header->e_ident[6] != 1) {
        printf("wrong ELF version\n");
        return 0;
    }

    if (header->e_ident[7] != 0) {
        printf("wrong ABI version\n");
        return 0;
    }

    if (header->e_type != ET_EXEC) {
        printf("ELF type not recognized\n");
        return 0;
    }

    if (header->e_machine != EM_386) {
        printf("only 32 bit x86 machines supported\n");
        return 0;
    }

    if (header->e_phoff == 0 || header->e_phnum <= 0 || header->e_phentsize != sizeof(Elf32_Phdr)) {
        printf("malformed program header\n");
        return 0;
    }

    if (header->e_phoff > size || header->e_phoff + (header->e_phnum * sizeof(Elf32_Phdr)) > size) {
        printf("program headers too big\n");
        return 0;
    }

    return 1;
}

void elf_print_info(const uint8_t* elf) {
    Elf32_Ehdr* header = (Elf32_Ehdr*)elf;

    printf("ELF Info\n");
    printf("-----------\n");
    printf("magic: ");
    printf("%x, %c, %c, %c\n", header->e_ident[0], header->e_ident[1], header->e_ident[2], header->e_ident[3]);

    printf("class: %d\n", header->e_ident[4]);
    printf("data: %d\n", header->e_ident[5]);
    printf("type: %d\n", header->e_type);
    printf("machine: %d\n", header->e_machine);
    printf("version: %d\n", header->e_version);
    printf("entry: %x\n", header->e_entry);
    printf("phoff: %x\n", header->e_phoff);
    printf("phentsize: %d\n", header->e_phentsize);
    printf("phnum: %d\n", header->e_phnum);
    printf("shoff: %x\n", header->e_shoff);
    printf("shentsize: %d\n", header->e_shentsize);
    printf("shnum: %d\n", header->e_shnum);
    printf("ehsize: %d\n", header->e_ehsize);
}

uint8_t elf_load(const uint8_t* elf, uint32_t size) {
    Elf32_Ehdr* header = (Elf32_Ehdr*)elf;
    for (uint32_t offset = header->e_phoff; offset < header->e_phoff + header->e_phentsize * header->e_phnum; offset += header->e_phentsize) {
        Elf32_Phdr* prog_header = (Elf32_Phdr*)((uint32_t)elf + offset);
        if (prog_header->p_type != PT_LOAD) {
            continue;
        }

        uint32_t start = prog_header->p_vaddr & ~(PAGE_SIZE - 1);
        uint32_t end = (prog_header->p_vaddr + prog_header->p_memsz + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

        for (uint32_t addr = start; addr < end; addr += PAGE_SIZE) {
            uint32_t frame = pmm_alloc_frame();
            if (!frame) {
                return 0;
            }

            map_user_page(addr, frame, PAGE_WRITE);
        }
        
        void* src = (void*)(elf + prog_header->p_offset);
        void* dst = (void*)prog_header->p_vaddr;
        memcpy(dst, src, prog_header->p_filesz);
        memset((void*)(prog_header->p_vaddr + prog_header->p_filesz), 0, prog_header->p_memsz - prog_header->p_filesz);
    }

    return 1;
}

__attribute__((naked, noreturn))
void elf_return_to_kernel(void) {
    __asm__ __volatile__(
        "mov current_process, %eax      \n\t"
        "mov 16(%eax), %esp              \n\t"
        "mov 20(%eax), %ebp              \n\t"
        "call elf_cleanup               \n\t"
        "cli                            \n\t"
        "1: hlt                         \n\t"
        "jmp 1b                         \n\t"
    );
}

void elf_cleanup() {
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

    tty();
}

uint32_t elf_init_stack() {
    // allocates 4 pages for the stack
    for (uint32_t addr = STACK_TOP - (PAGE_SIZE * 4); addr < STACK_TOP; addr += PAGE_SIZE) {
        uint32_t frame = pmm_alloc_frame();
        map_user_page((void*)addr, frame, PAGE_WRITE);
    }
    
    // zero out the memory
    uint32_t start = STACK_TOP - (PAGE_SIZE * 4);
    memset((void*)start, 0, PAGE_SIZE * 4);
   
    return start;
}

__attribute__((noreturn))
void enter_user_mode(uint32_t entry, uint32_t user_stack_top) {
    __asm__ __volatile__(
        "cli                    \n\t"

        /* load user data selectors into data segment registers */
        "mov %0, %%ax           \n\t"
        "mov %%ax, %%ds         \n\t"
        "mov %%ax, %%es         \n\t"
        "mov %%ax, %%fs         \n\t"
        "mov %%ax, %%gs         \n\t"

        /* build iret frame for ring 3 */
        "pushl %0               \n\t"  /* SS  */
        "pushl %1               \n\t"  /* ESP */
        "pushfl                 \n\t"  /* EFLAGS */
        "pushl %2               \n\t"  /* CS  */
        "pushl %3               \n\t"  /* EIP */

        "iret                   \n\t"
        :
        : "i"(USER_DS_RING3),
          "r"(user_stack_top),
          "i"(USER_CS_RING3),
          "r"(entry)
        : "ax", "memory"
    );

    __builtin_unreachable();
}

__attribute__((noreturn))
void elf_enter(uint32_t entry, uint32_t initial_esp) {
    asm volatile(
        "mov %0, %%esp    \n\t"
        "xor %%ebp, %%ebp \n\t"
        "jmp *%1          \n\t"
        :
        : "r"(initial_esp), "r"(entry)
        : "memory"
    );

    __builtin_unreachable();
}

void elf_execute(fs_node_t* elf) {
    uint32_t size;
    process_t* process = kmalloc(sizeof(process_t));
    process->file = elf;
    uint8_t* buf = elf_from_file(elf, &size);
    Elf32_Ehdr* header = (Elf32_Ehdr*)buf;
    
    if (!elf_validate(buf, size)) {
        return;
    }

    uint32_t entry = header->e_entry;
    process->entry = header->e_entry;
    

    if (!elf_load(buf, size)) {
        return; 
    }
    
    uint32_t stack_top = elf_init_stack();
    process->user_stack_top = stack_top;
    process->user_stack_bottom = stack_top - (PAGE_SIZE * 4);
    process->saved_kernel_esp = 0;
    asm volatile("mov %%esp, %0" : "=r"(process->saved_kernel_esp));
    asm volatile("mov %%ebp, %0" : "=r"(process->saved_kernel_ebp));
    
    current_process = process;
    enter_user_mode(entry, stack_top);
}



