#include "fs/console.h"

uint32_t console_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    (void)node;
    (void)offset;

    if (!buffer || size == 0) {
        log_error("Buffer is %p and size was %d\n", buffer, size); 
        return 0;
    } 
        

    while (!keyboard_buffer.ready) {
        asm volatile("sti; hlt" ::: "memory");
    }

    asm volatile("cli" ::: "memory");

    uint32_t len = keyboard_buffer.length;
    if (len >= size)
        len = size - 1;

    for (uint32_t i = 0; i < len; i++) {
        buffer[i] = (uint8_t)keyboard_buffer.data[i];
    }

    buffer[len] = '\0';

    keyboard_buffer.data[0] = '\0';
    keyboard_buffer.length = 0;
    keyboard_buffer.ready = 0;

    asm volatile("sti" ::: "memory");

    return len;
}

uint32_t console_writefile(fs_node_t* node, char* buffer, uint32_t offset, uint32_t size) {
    (void)node;
    (void)offset;

    for (uint32_t i = 0; i < size; i++) {
        terminal_printf("%c", buffer[i]); 
    }

    return size;
}

fs_node_t* create_console_node(fs_node_t* parent) {
    fs_node_t* node = kzmalloc(sizeof(fs_node_t));

    if (!node) {
        log_error("Node '%s' is null", parent->name);
        return NULL;
    } 
        

    strcpy(node->name, "console");
    node->flags = FS_DEV;
    node->inode = 2;
    node->size = 0;
    node->device = NULL;
    node->mount_parent = NULL;

    node->read = console_read;
    node->readdir = NULL;
    node->finddir = NULL;
    node->parent = NULL;
    node->createdir = NULL;
    node->createfile = NULL;
    node->writefile = console_writefile;

    return node;
}
