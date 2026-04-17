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
    printf("Heap end: %x\n", KHEAP_END);
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

    if (!file_name) {
        return;
    }
    
    fs_node_t* file = fs_finddir(fs_cwd, file_name);
    
    if (!file) {
        return;
    }

    char buffer[file->size];
    fs_read(file, 0, file->size, buffer);
    printf("%s\n", buffer);

    kfree(file);
}

void ls_handler(void* arg) {
    char* path = strtok(NULL, ' ');
    fs_node_t* dir;
    
    if (path != NULL) {
        dir = resolve_path(path);
    } else {
        dir = fs_cwd;
    }

    dirent_t* entry = fs_readdir(dir, 0);
    if (!entry) {
        return;
    }
    

    uint32_t idx = 1;
    
    while (entry != NULL) {
        printf("%s\t", entry->name);
        kfree(entry);
        
        entry = fs_readdir(dir, idx);
        idx++;
    }

    printf("\n");
}

void cd_handler(void* arg) {
    char* path = strtok(NULL, ' ');

    if (!path) {
        fs_cwd = fs_root;
        return;
    }

    fs_node_t* new_cwd = resolve_path(path);
    if (!new_cwd) {
        return;
    }

    kfree(fs_cwd);
    fs_cwd = new_cwd;
}

void pwd_handler(void* arg) {
    fs_node_t* start = fs_cwd;
    fs_node_t* path[10] = { start };
    uint8_t idx = 1;

    while (start->inode != fs_root->inode) {
        start = fs_parent(start);

        path[idx++] = start;
    }

    for (int i = idx - 1; i >= 0; i--) {
        printf("%s", path[i]->name);
    }
    
    printf("\n");

    for (uint8_t i = idx - 1; i >= 0; idx--) {
        kfree(path[i]);
    }
}

void mkdir_handler(void* arg) {
    char* name = strtok(NULL, ' ');
    if (!name) {
        return;
    }

    fs_cwd->createdir(fs_cwd, name); 
}

void touch_handler(void* arg) {
    char* name = strtok(NULL, ' ');
    if (!name) {
        return;
    }

    fs_cwd->createfile(fs_cwd, name, 10);
}

void write_handler(void* arg) {
    char* path = strtok(NULL, ' ');
    char* buffer = strtok(NULL, ' ');

    if (!path || ! buffer) {
        return;
    }

    fs_node_t* file = resolve_path_from(fs_cwd, path);
    file->writefile(file, buffer, 0, strlen(buffer));

    kfree(file);
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
    { "cd", cd_handler },
    { "pwd", pwd_handler },
    { "mkdir", mkdir_handler },
    { "touch", touch_handler },
    { "write", write_handler },
    { "exit", exit_handler }
};

const uint8_t num_commands = 14;
