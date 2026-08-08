#include "fs/devfs/dev_pts.h"

#include "exec/scheduler.h"

static uint8_t poll_pts_data(dev_file_t* file, uint32_t offset) {
    (void)file;
    (void)offset;

    uint8_t poll = POLLOUT;

    if (!queue_empty(&pty.input))
        poll |= POLLIN;

    return poll;
}

static int32_t write_pts_data(dev_file_t* file, uint8_t* buffer, uint32_t offset, uint32_t size) {
    (void)offset; 

    if (!file || !buffer)
        return EFAULT;

    for (uint32_t i = 0; i < size; i++) {
        enqueue(&pty.output, &buffer[i]);
    }

    if (size > 0)
        process_wake_blocked();

    return size;
}

static int32_t read_pts_data(dev_file_t* file, uint8_t* buffer, uint32_t offset, uint32_t size) {
    (void) offset;

    if (!file || !buffer)
        return EFAULT;

    if (queue_empty(&pty.input))
        return EAGAIN;

    if (size > queue_size(&pty.input))
        size = queue_size(&pty.input);

    for (uint32_t i = 0; i < size; i++) {
        char c = *(char*)dequeue(&pty.input);
        buffer[i] = c;
    }

    return size;
}

dev_file_t* create_pts_file(fs_node_t* parent, uint32_t inode) {
    if (!parent)
        return NULL;

    dev_file_t* file = kmalloc(sizeof(dev_file_t));

    if (!file)
        return NULL;

    file->parent = fs_node_clone(parent);
    strcpy(file->name, "pts");
    file->inode = inode;
    file->get_data = read_pts_data;
    file->write_data = write_pts_data;
    file->poll_data = poll_pts_data;

    return file;
}
