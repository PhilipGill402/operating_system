#ifndef INCLUDE_TTY_TTY_H_
#define INCLUDE_TTY_TTY_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "tty/commands.h"
#include "memory/heap.h"
#include "memory/physical_allocator.h"
#include "interrupts/keyboard.h"
#include "fs/fs.h"
#include "exec/elf.h"
#include "io/vga.h"
#include "timer.h"

extern uint32_t heap_curr; 
extern uint32_t heap_end;
extern pmm_t pmm;

void read_command();
void tty();

#endif // !INCLUDE_TTY_H_
