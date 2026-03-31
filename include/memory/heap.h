#ifndef INCLUDE_HEAP_H_
#define INCLUDE_HEAP_H_

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "paging.h"

// same as KERNEL_VIRT_MEM
#define KHEAP_START 0xC1000000
#define KHEAP_END   0xC2000000
#define HEAP_ALIGN 8

extern uint32_t heap_curr;
extern uint32_t heap_end;

void kheap_init();
void* kmalloc(size_t size);
void* kzmalloc(size_t size);

#endif // !INCLUDE_HEAP_H_


