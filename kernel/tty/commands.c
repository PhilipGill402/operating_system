#include "tty/commands.h"

void help_handler(void* arg) {
    for (uint8_t i = 0; i < num_commands; i++) {
        printf("%s\n", commands[i].cmd);
    } 
}

void echo_handler(void* arg) {
        char* cmd = (char*)arg; 
        cmd = strtok(NULL, ' '); 
        do {
            printf("%s ", cmd);
            cmd = strtok(NULL, ' ');
        } while (cmd);
        printf("\n");
}

void clear_handler(void* arg) {
    terminal_initialize(); 
}

void ticks_handler(void* arg) {
    printf("%d\n", timer_get_ticks());
}

void mem_handler(void* arg) {
    printf("Memory Info\n");
    printf("-----------\n");
    printf("Page size: %d bytes\n", PAGE_SIZE);
    printf("Total frames: %d\n", pmm.total_frames);
    printf("Used frames: %d\n", pmm.used_frames);
    printf("Free frames: %d\n", pmm.free_frames);
    printf("Bitmap bytes: %d\n\n", pmm.bitmap_bytes);

    printf("Heap start: %x\n", KHEAP_START);
    printf("Heap end: %x\n", heap_end);
    printf("Heap current: %x\n", heap_curr);
}

void allocpage_handler(void* arg) {
    printf("allocated a page\n"); 
    uint32_t frame = pmm_alloc_frame();
    if (!frame) {
        return 0;
    }

    uint32_t virt = next_free_virt;
    next_free_virt += PAGE_SIZE;

    map_page(virt, frame, 0);

    printf("Phys: %x\n", frame);
    printf("Virt: %x\n", virt);
}

void pagefault_handler(void* arg) {
    uint32_t* virt = alloc_kernel_page(0);
    *virt = 1;
}

void exit_handler(void* arg) {
    return;
}

void cat_handler(void* arg) {
    char* file_name = strtok(NULL, ' ');
    
    fs_node_t* file = fs_finddir(fs_root, file_name);
    char buffer[file->size];
    fs_read(file, 0, file->size, buffer);
    printf("%s", buffer);
}

void ls_handler(void* arg) {
    for (uint32_t i = 0; i < initrd_header->num_files; i++) {
        dirent_t entry = fs_readdir(fs_root, i);
        printf("%s\t", entry.name);
    }
    printf("\n");
}

const command_t commands[] = {
    { "help", help_handler },
    { "echo", echo_handler },
    { "clear", clear_handler },
    { "ticks", ticks_handler },
    { "mem", mem_handler },
    { "allocpage", allocpage_handler },
    { "pagefault", pagefault_handler },
    { "cat", cat_handler },
    { "ls", ls_handler },
    { "exit", exit_handler }
};

const uint8_t num_commands = 10;
