#ifndef INCLUDE_TTY_COMMANDS_H_
#define INCLUDE_TTY_COMMANDS_H_

#include <stdint.h>
#include "timer.h"
#include "memory/heap.h"
#include "memory/physical_allocator.h"
#include "fs/fs.h"
#include "fs/initrd.h"

typedef struct {
    char* cmd;
    void (*handler)(void*);
} command_t;

extern const command_t commands[];
extern const uint8_t num_commands;

#endif // !INCLUDE_TTY_COMMANDS_H_
