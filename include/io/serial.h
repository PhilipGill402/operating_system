#ifndef INCLUDE_SERIAL_H_
#define INCLUDE_SERIAL_H_

#include <stdint.h>
#include <stddef.h>
#include <arch/interrupts/port.h>



void serial_init();
void serial_write_char(char c, void* ctx);
void serial_write(const char* str);

#endif
