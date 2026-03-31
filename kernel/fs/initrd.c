#include "fs/initrd.h"

initrd_header_t* initrd_header;
initrd_file_header_t* initrd_file_headers;
initrd_superblock_t superblock;
initrd_node_t* node_table;
uint32_t num_nodes;

uint32_t initrd_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    initrd_file_header_t header = initrd_file_headers[node->inode];

    if (offset >= node->size) {
        return 0;
    }

    uint32_t bytes_to_read = size < (node->size - offset) ? size : (node->size - offset);
    
    uint8_t* file = (uint8_t*)initrd_header + header.offset + offset;

    memcpy(buffer, file, bytes_to_read);
    buffer[bytes_to_read] = '\0';
    
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
    initrd_header = (initrd_superblock_t*)addr;
    uint32_t files_added = 0;
    node_table = kmalloc(sizeof(initrd_node_t) * (initrd_header->num_files + 1));

    if (!node_table) {
        printf("couldn't allocate space for the node table\n");
        return NULL;
    }

    initrd_file_headers = (initrd_file_header_t*)(addr + sizeof(initrd_header_t));
    
    initrd_node_t root;
    memset(&root, 0, sizeof(initrd_node_t));
    root.id = 0;
    root.type = INITRD_NODE_DIR;
    root.parent_id = INITRD_INVALID_NODE;
    strcpy(root.name, "/");
    node_table[files_added++] = root;
    
    for (uint32_t i = 0; i < initrd_header->num_files; i++) {
        initrd_file_header_t file_header = initrd_file_headers[i];

        initrd_node_t file;
        memset(&file, 0, sizeof(initrd_node_t));
        file.id = files_added;
        file.type = INITRD_NODE_FILE;
        file.parent_id = file_header.id;
        strncpy(file.name, file_header.name, INITRD_NAME_MAX);
        file.size = file_header.size;
        file.offset = file_header.offset;
        node_table[files_added++] = file;
    }

    num_nodes = files_added;
    
    return &node_table[0];
}


