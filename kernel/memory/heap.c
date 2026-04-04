#include "memory/heap.h"

static heap_t heap;

static uint32_t align_up(uint32_t value, uint32_t align) {
    return (value + align - 1) & ~(align - 1);
}

bool is_free(block_t* block){
    return !block->allocated;
}

size_t block_size(block_t* block){
    return block->size;
}

block_t* get_next_block(block_t* block){
    uint8_t* current = (uint8_t*)block;
    size_t step = block_size(block);
    uint8_t* next = current + step;

    if (next >= heap.end){
        return NULL;
    }
    
    return (block_t*)next;
}

void coalesce(block_t* block){
    block_t* next = get_next_block(block);
    if (next == NULL){
        return;
    }
    
    if (!next->allocated){
        block->size += next->size;
    }
}

/* Main Functions */
void init_heap(){
    for (uint32_t addr = KHEAP_START; addr < KHEAP_END; addr += PAGE_SIZE) {
        uint32_t frame = pmm_alloc_frame();
        if (!frame) {
            return;
        }

        map_page(addr, frame, PAGE_WRITE);
    }

    heap.ptr = (uint8_t*)KHEAP_START;
    heap.size = KHEAP_END - KHEAP_START;
    heap.end = KHEAP_END; 

    block_t* block = (block_t*)heap.ptr;
    block->size = heap.size;
    block->allocated = false;
}

void* kmalloc(size_t size){
    //align to 8 byte increments
    size_t total_size = align_up(sizeof(block_t) + size, 8);
    
    //makes sure the requested size is not bigger than the arena
    if (total_size > (heap.size - sizeof(block_t))){
        return NULL;
    }

    //finds the first fit block 
    block_t* block = (block_t*)heap.ptr;
    

    while ((uint8_t*)block < (uint8_t*)heap.end){
        //makes sure that the block is not allocated and has enough size 
        if (!block->allocated && block->size >= total_size){
            size_t unneeded = block->size - total_size;

            //checks the remaining number of bytes to see if its enough to create a new block with enough space for 8 bytes
            if (unneeded >= sizeof(block_t) + 8){
                //changes the block's properties to properly allocate it
                block->size = total_size;
                block->allocated = true;
                
                //divides the old block into the the new one and the remainder
                block_t* new_block = (block_t*)((uint8_t*)block + block->size);
                
                //sets the size of the new block and sets its allocation flag to false
                new_block->size = unneeded;
                new_block->allocated = false;
            } else {
                //if there is not enough remaining bytes then just go ahead and return the entire block
                block->allocated = true;
            }
            
            //returns the first address after the block header
            return (void*)((uint8_t*)block + sizeof(block_t));
        } 
        
        //gets the next block
        block_t* next = get_next_block(block);
        
        //we reached the end of the arena 
        if (next == NULL){
            break;
        }
        
        //if the next block was not null then set the current block to it
        block = next;
    } 
    
    //we reached the end of the arena so therefore we cannot allocate any memory
    return NULL;
}

void kfree(void* ptr) {
    block_t* block = (block_t*)((uint8_t*)ptr - sizeof(block_t));
    block->allocated = false;
    
    //combines the current block with the next if the next isn't allocated
    coalesce(block);
}

void* kzmalloc(size_t size) {
    void* ptr = kmalloc(size);
    
    if (!ptr) {
        return NULL;
    }

    memset(ptr, 0, size);
    return ptr;
}

void* krealloc(void* ptr, size_t size) {
    void* new_ptr = kmalloc(size);
    block_t* block = (block_t*)((uint8_t*)ptr - sizeof(block_t));
    
    size_t old_payload_size = block->size - sizeof(block_t);
    size_t copy_size = old_payload_size < size ? old_payload_size : size;
    memcpy(new_ptr, ptr, copy_size);

    kfree(ptr);
    ptr = new_ptr;

    return ptr;
}



