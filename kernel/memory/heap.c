#include "memory/heap.h"

static uint32_t heap_curr = KHEAP_START;
static uint32_t heap_end = KHEAP_START;

static uint32_t align_up(uint32_t value, uint32_t align) {
    return (value + align - 1) & ~(align - 1);
}

uint8_t grow_heap() {
    uint32_t* page = alloc_kernel_page(PAGE_WRITE);

    if (!page) {
        return 0;
    }

    heap_end += PAGE_SIZE;

    return 1;
}

void* kmalloc(size_t size) {
    size = align_up(size, HEAP_ALIGN);

    while (heap_curr + size > heap_end) {
        if (!grow_heap()) {
            return NULL;
        }
    }

    void* ptr = (void*)heap_curr;
    heap_curr += size;

    return ptr;
}

void* kzmalloc(size_t size) {
    void* ptr = kmalloc(size);
    
    if (!ptr) {
        return NULL;
    }

    memset(ptr, 0, size);
    return ptr;
}


