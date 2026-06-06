#include "fs/procfs/proc_cpuinfo.h"

static int32_t get_cpuinfo_data(proc_file_t* proc, uint8_t* buffer, uint32_t offset, uint32_t size) {
    log_debug("made it\n"); 
    return 0;
}

proc_file_t* create_cpuinfo_file(fs_node_t* parent, uint32_t inode) {
    if (!parent)
        return NULL;

    proc_file_t* file = kmalloc(sizeof(proc_file_t));
    if (!file)
        return NULL;


    file->parent = fs_node_clone(parent);
    strcpy(file->name, "cpuinfo");
    file->inode = inode;
    file->get_data = get_cpuinfo_data;
    
    return file;
}
