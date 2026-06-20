#include "fs/devfs/dev_serial.h"

static uint8_t poll_serial_data(dev_file_t* file, uint32_t offset) {
    (void)file;
    (void)offset;

    return POLLOUT;
}

static int32_t write_serial_data(dev_file_t* file, uint8_t* buffer, uint32_t offset, uint32_t size) {
    (void)offset; 

    if (!file || !buffer)
        return 0;

    for (uint32_t i = 0; i < size; i++) {
        serial_write_char(buffer[i], NULL);
    }

    return size;
}

dev_file_t* create_serial_file(fs_node_t* parent, uint32_t inode) {
    if (!parent)
        return NULL;

    dev_file_t* file = kmalloc(sizeof(dev_file_t));

    if (!file)
        return NULL;
    
    file->parent = fs_node_clone(parent);
    strcpy(file->name, "serial");
    file->inode = inode;
    file->get_data = NULL;
    file->write_data = write_serial_data;
    file->poll_data = poll_serial_data;

    return file;
}
