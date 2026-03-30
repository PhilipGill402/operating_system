#include "fs/initrd.h"

fs_node_t root;
fs_node_t* files;
initrd_header_t* initrd_header;
initrd_file_header_t* initrd_file_headers;

uint32_t initrd_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    initrd_file_header_t header = initrd_file_headers[node->inode];

    if (offset >= node->size) {
        return 0;
    }

    uint32_t bytes_to_read = size < (node->size - offset) ? size : (node->size - offset);
    
    uint8_t* file = (uint8_t*)initrd_header + header.offset + offset;

    memcpy(buffer, file, bytes_to_read);
    
    return bytes_to_read;
}

dirent_t initrd_readdir(fs_node_t* node, uint32_t index) {
    dirent_t entry;
    if (index >= initrd_header->num_files) {
        entry.inode = (uint32_t)-1;     
        return entry;
    }
    

    fs_node_t file = files[index];

    strcpy(entry.name, file.name);
    entry.inode = file.inode;

    return entry;
}

fs_node_t* initrd_finddir(fs_node_t* node, char* name) {
    (void)node;

    for (uint32_t i = 0; i < initrd_header->num_files; i++) {
        if (strcmp(files[i].name, name) == 0) {
            return &files[i];
        }
    }

    return NULL;
}

fs_node_t* initrd_init(uint32_t addr) {
    initrd_header = (initrd_header_t*)addr;

    initrd_file_headers = (initrd_file_header_t*)(addr + sizeof(initrd_header_t));
    
    memset(&root, 0, sizeof(fs_node_t));
    strcpy(root.name, "/"); 
    root.flags = FS_DIRECTORY;
    root.readdir = initrd_readdir;
    root.finddir = initrd_finddir;
    
    files = kmalloc(sizeof(fs_node_t) * initrd_header->num_files);
    
    uint8_t* raw = (uint8_t*)(addr);
    printf("initrd raw after kmalloc: %x %x %x %x\n", raw[0], raw[1], raw[2], raw[3]);

    for (uint32_t i = 0; i < initrd_header->num_files; i++) {
        initrd_file_header_t file_header = initrd_file_headers[i];

        fs_node_t file;
        memset(&file, 0, sizeof(fs_node_t));
        strcpy(file.name, file_header.name);
        file.flags = FS_FILE;
        file.size = file_header.size;
        file.read = initrd_read;
        file.inode = i;
        //printf("%d\n", i);
        files[i] = file;
    }
    printf("made it\n");
    return &root;
}


