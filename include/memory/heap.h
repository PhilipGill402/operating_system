#ifndef INCLUDE_HEAP_H_
#define INCLUDE_HEAP_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "paging.h"

// same as KERNEL_VIRT_MEM
#define KHEAP_START 0xC1000000
#define KHEAP_END   0xC2000000
#define HEAP_ALIGN 8
#define PAGE_SIZE 4096

void kheap_init();
void* kmalloc(size_t size);
void* kzmalloc(size_t size);

typedef uintptr_t word_t;

typedef struct block_t {
    size_t size;
    bool allocated;
    //pads block to be 16 bytes
    uint8_t _pad[7];
} block_t;

typedef struct heap_t {
    uint8_t* ptr;
    size_t size;
    uint8_t* end;
} heap_t;

void init_heap();
void* kmalloc(size_t size);
void* kzmalloc(size_t size);
void kfree(void* ptr);

#endif // !INCLUDE_HEAP_H_


