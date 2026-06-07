#ifndef INCLUDE_HEAP_H_
#define INCLUDE_HEAP_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "memory/paging.h"

// same as KERNEL_VIRT_MEM
#define KHEAP_START 0xC2000000
#define KHEAP_END   0xC3000000
#define HEAP_ALIGN 8
#define PAGE_SIZE 4096

#define KHEAP_CANARY 0xDEADBEEF

typedef uintptr_t word_t;

typedef struct kblock_t {
    size_t size;
    bool allocated;
    const char* file;
    int line;
    //pads block to be 16 bytes
    uint8_t _pad[7];
} kblock_t;

typedef struct heap_t {
    uint8_t* ptr;
    size_t size;
    uint8_t* end;
} heap_t;

void* kmalloc(size_t size);
void* kzmalloc(size_t size);
void kfree(void* ptr);

void* kmalloc_debug(size_t size, const char* file, int line);
void* kzmalloc_debug(size_t size, const char* file, int line);
void kfree_debug(void* ptr, const char* file, int line);

#ifdef MALLOC_DEBUG

#define kmalloc(size)   kmalloc_debug(size, __FILE__, __LINE__)
#define kzmalloc(size)  kzmalloc_debug(size, __FILE__, __LINE__)
#define kfree(ptr)      kfree_debug(ptr, __FILE__, __LINE__)

#endif

void* krealloc(void* ptr, size_t size);

void init_heap();

#endif // !INCLUDE_HEAP_H_


