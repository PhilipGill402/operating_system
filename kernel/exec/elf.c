#include "exec/elf.h"

uint8_t* elf_from_file(fs_node_t* elf, uint32_t* size) {
    uint8_t* buffer = kmalloc(elf->size);
    fs_read(elf, 0, elf->size, buffer);
    *size = elf->size;

    return buffer;
}

uint8_t elf_validate(const uint8_t* elf, uint32_t size) {
    if (!elf) {
        printf("elf data is NULL\n");
        return 0;
    }

    if (size < sizeof(Elf32_Ehdr)) {
        printf("Elf not large enough for ELF header\n"); 
        return 0;
    }
    
    Elf32_Ehdr* header = (Elf32_Ehdr*)elf; 

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

uint8_t elf_load(const uint8_t* elf, uint32_t size, process_t* process) {
    Elf32_Ehdr* header = (Elf32_Ehdr*)elf;
    uint8_t count = 0;

    for (uint32_t offset = header->e_phoff;
         offset < header->e_phoff + header->e_phentsize * header->e_phnum;
         offset += header->e_phentsize) {

        Elf32_Phdr* ph = (Elf32_Phdr*)((uint32_t)elf + offset);

        if (ph->p_type != PT_LOAD) {
            continue;
        }

        if (count >= MAX_SEGMENTS) {
            printf("too many segments\n");
            return 0;
        }

        if (ph->p_offset > size || ph->p_filesz > size - ph->p_offset) {
            printf("segment exceeds ELF file size\n");
            return 0;
        }

        if (ph->p_memsz < ph->p_filesz) {
            printf("segment memsz smaller than filesz\n");
            return 0;
        }

        uint32_t seg_start = ph->p_vaddr;
        uint32_t seg_end   = ph->p_vaddr + ph->p_memsz;

        uint32_t page_start = seg_start & ~(PAGE_SIZE - 1);
        uint32_t page_end   = (seg_end + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

        uint32_t* pd = temp_map_phys0(process->page_directory_phys);

        for (uint32_t vaddr = page_start; vaddr < page_end; vaddr += PAGE_SIZE) {
            uint32_t frame = pmm_alloc_frame();
            if (!frame) {
                return 0;
            }

            map_user_page(pd, vaddr, frame, PAGE_WRITE);

            uint8_t* page = (uint8_t*)temp_map_phys1(frame);
            memset(page, 0, PAGE_SIZE);

            uint32_t copy_start = vaddr;
            if (copy_start < ph->p_vaddr) {
                copy_start = ph->p_vaddr;
            }

            uint32_t copy_end = vaddr + PAGE_SIZE;
            if (copy_end > ph->p_vaddr + ph->p_filesz) {
                copy_end = ph->p_vaddr + ph->p_filesz;
            }

            if (copy_start < copy_end) {
                uint32_t dst_off = copy_start - vaddr;
                uint32_t src_off = ph->p_offset + (copy_start - ph->p_vaddr);
                uint32_t nbytes = copy_end - copy_start;

                memcpy(page + dst_off, elf + src_off, nbytes);
            }
        }

        process->mem_ranges[count++] = (mem_range_t){ .start = page_start, .end = page_end };
    }

    process->num_ranges = count;
    return 1;
}

uint32_t elf_init_stack(process_t* process) {
    uint32_t stack_bottom = STACK_TOP - (PAGE_SIZE * 4);
    uint32_t stack_top = STACK_TOP;

    uint32_t* pd = temp_map_phys0(process->page_directory_phys);

    for (uint32_t addr = stack_bottom; addr < stack_top; addr += PAGE_SIZE) {
        uint32_t frame = pmm_alloc_frame();
        if (!frame) {
            return 0;
        }

        map_user_page(pd, addr, frame, PAGE_WRITE);

        uint8_t* page = (uint8_t*)temp_map_phys1(frame);
        memset(page, 0, PAGE_SIZE);
    }

    process->user_stack_top = stack_top;
    process->user_stack_bottom = stack_bottom;

    return 1;
}

process_t* process_create_from_elf(fs_node_t* elf) {
    uint32_t size;
    process_t* process = kzmalloc(sizeof(process_t));

    process_init_file_descriptors(process);
    fs_node_t* process_cwd = kmalloc(sizeof(fs_node_t));
    memcpy(process_cwd, fs_cwd, sizeof(fs_node_t));
    process->cwd = process_cwd;
    
    uint8_t* buf = elf_from_file(elf, &size);
    
    if (!elf_validate(buf, size)) {
        kfree(buf);
        process_destroy(process); 
        return NULL;
    }

    Elf32_Ehdr* header = (Elf32_Ehdr*)buf;
    uint32_t entry = header->e_entry;
    process->entry = header->e_entry;
    process->page_directory_phys = process_create_page_directory();
    if (!process->page_directory_phys) {
        kfree(buf);
        process_destroy(process);
        return NULL;
    }
    
    if (!elf_load(buf, size, process)) {
        kfree(buf);
        process_destroy(process);
        return NULL; 
    }

    if (!elf_init_stack(process)) {
        kfree(buf);
        process_destroy(process);
        return NULL;
    }
    
    
    process->kernel_stack_bottom = (uint32_t)kmalloc(KERNEL_STACK_SIZE);
    process->kernel_stack_top = process->kernel_stack_bottom + KERNEL_STACK_SIZE;
    process->pid = num_processes++;
    process->ticks_left = DEFAULT_MAX_TICKS;
    process->state = PROC_READY;

    if (!process->page_directory_phys) {
        kfree(buf);
        process_destroy(process);
        return NULL;
    }

    process_init_trapframe(process);
    
    kfree(buf);
    
    return process;
}

void elf_execute(fs_node_t* elf) {
    process_t* process = process_create_from_elf(elf);

    if (!process) {
        return;
    }
     
    enqueue(&current_processes, &process);
}



