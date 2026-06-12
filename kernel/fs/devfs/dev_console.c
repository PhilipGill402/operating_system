#include "fs/devfs/dev_console.h"

#include <log.h>

int32_t get_console_data(dev_file_t* file, uint8_t* buffer, uint32_t offset, uint32_t size) {
    (void)file;
    (void)offset;
    
    return 0;
}

int32_t write_console_data(dev_file_t* file, uint8_t* buffer, uint32_t offset, uint32_t size) {
    (void)file;
    (void)offset;
    
    for (uint32_t i = 0; i < size; i++) {
        log_debug("%c\n", buffer[i]);
    }

    return (int32_t)size;
}

dev_file_t* create_console_file(fs_node_t* parent, uint32_t inode) {
    if (!parent)
        return NULL;

    dev_file_t* file = kmalloc(sizeof(dev_file_t));

    if (!file)
        return NULL;
    
    file->parent = fs_node_clone(parent);
    strcpy(file->name, "console");
    file->inode = inode;
    file->get_data = get_console_data;
    file->write_data = write_console_data;

    return file;
}
