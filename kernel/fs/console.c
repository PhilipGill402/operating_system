#include "fs/console.h"

uint32_t console_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    (void)node;
    (void)offset;

    if (!buffer || size == 0) return 0;

    while (!input_buffer_ready()) {
        // busy wait for now
        asm volatile("sti; hlt");

    }
    uint32_t count = 0;
    while (count < size && count < input_buffer_length()) {
        buffer[count] = (uint8_t)input_buffer_data()[count];
        count++;
    }

    input_buffer_reset();
    return count;
}

void console_writefile(fs_node_t* node, char* buffer, uint32_t offset, uint32_t size) {
    (void)node;
    (void)offset;

    for (uint32_t i = 0; i < size; i++) {
        printf("%c", buffer[i]); 
    }
}

fs_node_t* create_console_node(fs_node_t* parent) {
    fs_node_t* node = kzmalloc(sizeof(fs_node_t));

    if (!node) return NULL;

    strcpy(node->name, "console");
    node->flags = FS_DEV;
    node->inode = 2;
    node->size = 0;
    node->device = NULL;

    node->read = console_read;
    node->readdir = NULL;
    node->finddir = NULL;
    node->parent = NULL;
    node->createdir = NULL;
    node->createfile = NULL;
    node->writefile = console_writefile;

    return node;
}
