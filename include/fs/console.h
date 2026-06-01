#ifndef INCLUDE_FS_CONSOLE_H_
#define INCLUDE_FS_CONSOLE_H_

#include <stdio.h>
#include <log.h>
#include "fs/fs_types.h"
#include "memory/heap.h"
#include "interrupts/keyboard.h"

fs_node_t* create_console_node(fs_node_t* parent);

#endif // !INCLUDE_FS_CONSOLE_H_
