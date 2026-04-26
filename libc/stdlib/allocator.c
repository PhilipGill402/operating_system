#include <stdlib.h>

#include <stdio.h>

static uint8_t* heap_head = NULL;
static uint8_t* heap_tail = NULL;

/* Helper Functions */
size_t align(size_t size, size_t alignment){
    return (size + (alignment - 1)) & ~(alignment - 1);
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
    uint8_t* next = current + step + sizeof(block_t);

    if (next >= heap_tail){
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
        block->size += sizeof(block_t) + next->size;
    }
}

/* Main Functions */
void* malloc(size_t size){
    //align to 8 byte increments
    size_t total_size = align(size, 8);
    // first malloc: sets the heap head to the break in the heap
    if (!heap_head) {
        heap_head = (uint8_t*)sbrk(PAGE_SIZE);
        if (!heap_head) return NULL;
        heap_tail = heap_head + PAGE_SIZE;
        block_t* first_block = (block_t*)heap_head;
        first_block->size = PAGE_SIZE - sizeof(block_t);
        first_block->allocated = 0;
    }
    
    //finds the first fit block 
    block_t* block = (block_t*)heap_head;
    
    while ((uint8_t*)block < heap_tail){
        //makes sure that the block is not allocated and has enough size 
        if (!block->allocated && block->size >= total_size){
            size_t unneeded = block->size - total_size;

            //checks the remaining number of bytes to see if its enough to create a new block with enough space for 8 bytes
            if (unneeded >= sizeof(block_t) + 8){
                //changes the block's properties to properly allocate it
                block->size = total_size;
                block->allocated = true;
                
                //divides the old block into the the new one and the remainder
                block_t* new_block = (block_t*)((uint8_t*)block + sizeof(block_t) + block->size);
                
                //sets the size of the new block and sets its allocation flag to false
                new_block->size = unneeded - sizeof(block_t);
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
        
        //we reached the end of the heap
        if (next == NULL){
            break;
        }
        
        //if the next block was not null then set the current block to it
        block = next;
    }

    // couldn't find a big enough block
    void* ptr = sbrk(total_size + (2*sizeof(block_t)) + PAGE_SIZE);
    if (!ptr) return NULL;
    
    // create header and return pointer to data
    block_t* header = (block_t*)ptr;
    header->allocated = 1;
    header->size = total_size;

    // because we no longer have any memory left, we extend the memory by a page
    uint8_t* new_mem = (uint8_t*)ptr + sizeof(block_t) + total_size; 
    block_t* new_header = (block_t*)new_mem;
    new_header->size = PAGE_SIZE;
    new_header->allocated = 0;
    heap_tail = (uint8_t*)new_header + sizeof(block_t) + new_header->size;
    
    //we reached the end of the heap so therefore we cannot allocate any memory
    return (void*)((uint8_t*)ptr + sizeof(block_t));
}

void free(void* ptr) {
    if (!ptr) return;
    
    block_t* block = (block_t*)((uint8_t*)ptr - sizeof(block_t));
    block->allocated = false;
    
    //combines the current block with the next if the next isn't allocated
    coalesce(block);
}

void* realloc(void* ptr, size_t size) {
    if (ptr == NULL) return malloc(size);
    if (size == 0) {
        free(ptr);
        return NULL;
    }

    void* new_ptr = malloc(size);
    if (!new_ptr) return NULL;

    block_t* block = (block_t*)((uint8_t*)ptr - sizeof(block_t));
    size_t old_size = block->size;
    size_t copy_size = old_size < size ? old_size : size;
    memcpy(new_ptr, ptr, copy_size);
    free(ptr);
    ptr = new_ptr;

    return ptr;
}
