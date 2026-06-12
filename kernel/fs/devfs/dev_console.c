#include "fs/devfs/dev_console.h"

#include <log.h>

int32_t get_console_data(dev_file_t* file, uint8_t* buffer, uint32_t offset, uint32_t size) {
    (void)file;
    (void)offset;
    
    if (!buffer || size == 0) {
        log_error("Buffer is %p and size was %d\n", buffer, size); 
        return 0;
    } 
        

    while (!keyboard_buffer.ready) {
        asm volatile("sti; hlt" ::: "memory");
    }

    asm volatile("cli" ::: "memory");

    int32_t len = (int32_t)keyboard_buffer.length;
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
