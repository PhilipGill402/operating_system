#include "fs/devfs/dev_ptm.h"
#include "exec/scheduler.h"

static uint8_t poll_ptm_data(dev_file_t* file, uint32_t offset) {
    (void)file;
    (void)offset;

    uint8_t poll = POLLOUT;

    if (!queue_empty(&pty.output))
        poll |= POLLIN;

    return poll;
}

static int32_t write_ptm_data(dev_file_t* file, uint8_t* buffer, uint32_t offset, uint32_t size) {
    (void)offset; 

    if (!file || !buffer)
        return EFAULT;

    for (uint32_t i = 0; i < size; i++) {
        enqueue(&pty.input, &buffer[i]);
    }

    if (size > 0)
        process_wake_blocked();

    return size;
}

static int32_t read_ptm_data(dev_file_t* file, uint8_t* buffer, uint32_t offset, uint32_t size) {
    (void) offset;

    if (!file || !buffer)
        return EFAULT;

    if (queue_empty(&pty.output))
        return EAGAIN;

    if (size > queue_size(&pty.output))
        size = queue_size(&pty.output);

    for (uint32_t i = 0; i < size; i++) {
        char c = *(char*)dequeue(&pty.output);
        buffer[i] = c;
    }

    return size;
}

dev_file_t* create_ptm_file(fs_node_t* parent, uint32_t inode) {
    if (!parent)
        return NULL;

    dev_file_t* file = kmalloc(sizeof(dev_file_t));

    if (!file)
        return NULL;

    file->parent = fs_node_clone(parent);
    strcpy(file->name, "ptm");
    file->inode = inode;
    file->get_data = read_ptm_data;
    file->write_data = write_ptm_data;
    file->poll_data = poll_ptm_data;

    return file;
}
