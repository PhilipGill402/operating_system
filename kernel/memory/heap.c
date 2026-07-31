#include "memory/heap.h"

#ifdef MALLOC_DEBUG
#undef kmalloc
#undef kzmalloc
#undef kfree
#endif

static heap_t heap;
static uint32_t num_allocs = 0;
static uint32_t num_frees = 0;

static uint32_t align_up(uint32_t value, uint32_t align) {
    return (value + align - 1) & ~(align - 1);
}

static uint32_t* block_canary(kblock_t* block) {
    return (uint32_t*)((uint8_t*)block + block->size - sizeof(uint32_t));
}

static bool check_canary(kblock_t* block) {
    return *block_canary(block) == KHEAP_CANARY;
}

static size_t block_payload_size(kblock_t* block) {
    return block->size - sizeof(kblock_t) - sizeof(uint32_t);
}

static bool ptr_in_heap(void* ptr) {
    return (uint8_t*)ptr >= heap.ptr && (uint8_t*)ptr < heap.end;
}

static void write_canary(kblock_t* block) {
    *block_canary(block) = KHEAP_CANARY;
}

static bool valid_block_basic(kblock_t* block) {
    if (!block) {
        return false;
    }

    if (!ptr_in_heap(block)) {
        log_error("heap: block outside heap: %x\n", block);
        return false;
    }

    if (block->size == 0) {
        log_error("heap: zero-size block at %x\n", block);
        return false;
    }

    if (block->size < sizeof(kblock_t) + sizeof(uint32_t)) {
        log_error("heap: block too small at %x size=%d\n", block, block->size);
        return false;
    }

    if (block->size % 8 != 0) {
        log_error("heap: unaligned block at %x size=%d\n", block, block->size);
        return false;
    }

    if ((uint8_t*)block + block->size > heap.end) {
        log_error("heap: block extends past heap: block=%x size=%d heap_end=%x\n",
                  block, block->size, heap.end);
        return false;
    }

    return true;
}



bool is_free(kblock_t* block){
    return !block->allocated;
}

size_t block_size(kblock_t* block){
    return block->size;
}

kblock_t* get_next_block(kblock_t* block){
    if (!valid_block_basic(block)) return NULL; 

    uint8_t* current = (uint8_t*)block;
    uint8_t* next = current + block->size;

    if (next >= heap.end){
        return NULL;
    }
    
    return (kblock_t*)next;
}

void coalesce(kblock_t* block){
    kblock_t* next = get_next_block(block);
    if (next == NULL){
        return;
    }
    
    if (!next->allocated){
        block->size += next->size;
        write_canary(block);
    }

}

/* Main Functions */
void init_heap(){
    for (uint32_t addr = KHEAP_START; addr < KHEAP_END; addr += PAGE_SIZE) {
        uint32_t frame = pmm_alloc_frame();
        if (!frame) {
            return;
        }

        arch_page_map(arch_kernel_address_space(), addr, frame, ARCH_PAGE_WRITE);
    }

    heap.ptr = (uint8_t*)KHEAP_START;
    heap.size = KHEAP_END - KHEAP_START;
    heap.end = (uint8_t*)KHEAP_END; 

    kblock_t* block = (kblock_t*)heap.ptr;
    block->size = heap.size;
    block->allocated = false;
    block->file = "init_heap";
    block->line = 0;

    write_canary(block);
}

void* kmalloc(size_t size){
    //align to 8 byte increments
    size_t total_size = align_up(sizeof(kblock_t) + size + sizeof(uint32_t), HEAP_ALIGN);
    
    //makes sure the requested size is not bigger than the arena
    if (total_size > (heap.size - sizeof(kblock_t))){
        return NULL;
    }

    //finds the first fit block 
    kblock_t* block = (kblock_t*)heap.ptr;
    

    while ((uint8_t*)block < (uint8_t*)heap.end){
        //makes sure that the block is not allocated and has enough size 
        if (!block->allocated && block->size >= total_size){
            size_t unneeded = block->size - total_size;

            //checks the remaining number of bytes to see if its enough to create a new block with enough space for 8 bytes
            if (unneeded >= sizeof(kblock_t) + 8){
                //changes the block's properties to properly allocate it
                block->size = total_size;
                block->allocated = true;
                block->file = "unknown";
                block->line = 0;
                write_canary(block);
                
                //divides the old block into the the new one and the remainder
                kblock_t* new_block = (kblock_t*)((uint8_t*)block + block->size);
                
                //sets the size of the new block and sets its allocation flag to false
                new_block->size = unneeded;
                new_block->allocated = false;
                new_block->file = "free";
                new_block->line = 0;
                write_canary(new_block);
            } else {
                //if there is not enough remaining bytes then just go ahead and return the entire block
                block->allocated = true;
                block->file = "unknown";
                block->line = 0;
                write_canary(block);
            }
            
            //returns the first address after the block header
            return (void*)((uint8_t*)block + sizeof(kblock_t));
        } 
        
        //gets the next block
        kblock_t* next = get_next_block(block);
        
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
    if (!ptr) return; 

    kblock_t* block = (kblock_t*)((uint8_t*)ptr - sizeof(kblock_t));
    block->allocated = false;
    
    //combines the current block with the next if the next isn't allocated
    coalesce(block);
    write_canary(block);
}

void* kzmalloc(size_t size) {
    log_debug("kzmalloc\n"); 
    void* ptr = kmalloc(size);
    
    if (!ptr) {
        return NULL;
    }

    memset(ptr, 0, size);
    return ptr;
}

void* krealloc(void* ptr, size_t size) {
    void* new_ptr = kmalloc(size);
    kblock_t* block = (kblock_t*)((uint8_t*)ptr - sizeof(kblock_t));
    
    size_t old_payload_size = block->size - sizeof(kblock_t);
    size_t copy_size = old_payload_size < size ? old_payload_size : size;
    memcpy(new_ptr, ptr, copy_size);

    kfree(ptr);
    ptr = new_ptr;

    return ptr;
}

bool heap_check(const char* where) {
    kblock_t* block = (kblock_t*)heap.ptr;
    kblock_t* prev = NULL;

    while ((uint8_t*)block < heap.end) {
        if (!ptr_in_heap(block)) {
            log_error("heap_check(%s): block outside heap: %x\n", where, block);
            return false;
        }

        if (block->size == 0) {
            log_error("heap_check(%s): ZERO SIZE BLOCK at %x\n", where, block);

            if (prev) {
                log_error("previous block=%x size=%d allocated=%d allocated_at=%s:%d\n",
                          prev, prev->size, prev->allocated, prev->file, prev->line);
            }

            return false;
        }

        if (block->size < sizeof(kblock_t) + sizeof(uint32_t)) {
            log_error("heap_check(%s): tiny block at %x size=%d\n",
                      where, block, block->size);
            return false;
        }

        if ((uint8_t*)block + block->size > heap.end) {
            log_error("heap_check(%s): block runs past heap block=%x size=%d\n",
                      where, block, block->size);
            return false;
        }

        if (block->allocated && !check_canary(block)) {
            log_error("heap_check(%s): CANARY CORRUPTED\n", where);
            log_error("block=%x size=%d payload=%d allocated_at=%s:%d canary=%x expected=%x\n",
                      block,
                      block->size,
                      block_payload_size(block),
                      block->file,
                      block->line,
                      *block_canary(block),
                      KHEAP_CANARY);
            return false;
        }

        uint8_t* next = (uint8_t*)block + block->size;

        if (next == heap.end) {
            return true;
        }

        prev = block;
        block = (kblock_t*)next;
    }

    return true;
}

void heap_dump(void) {
    log_debug("---- HEAP DUMP ----\n");

    kblock_t* block = (kblock_t*)heap.ptr;
    uint32_t index = 0;

    while ((uint8_t*)block < heap.end) {
        log_debug("[%d] block=%x size=%d payload=%d allocated=%d file=%s line=%d\n",
                  index,
                  block,
                  block->size,
                  block_payload_size(block),
                  block->allocated,
                  block->file,
                  block->line);

        block = (kblock_t*)((uint8_t*)block + block->size);
        index++;
    }

    log_debug("---- END HEAP DUMP ----\n");
}

void* kmalloc_debug(size_t size, const char* file, int line) {
    num_allocs++; 
    log_debug("Malloc called at %s:%d for %u bytes\n", file, line, size); 
    if (size == 0) {
        return NULL;
    }

    /*
     * Total block includes:
     * header + requested payload + canary
     */
    size_t total_size = align_up(sizeof(kblock_t) + size + sizeof(uint32_t), 8);

    if (total_size > heap.size) {
        log_error("kmalloc too large: size=%d total=%d at %s:%d\n",
                  size, total_size, file, line);
        return NULL;
    }

    kblock_t* block = (kblock_t*)heap.ptr;

    while ((uint8_t*)block < heap.end) {
        if (!valid_block_basic(block)) {
            log_error("kmalloc: heap corrupted while allocating size=%d at %s:%d\n",
                      size, file, line);
            //heap_dump();
            return NULL;
        }

        if (!block->allocated && block->size >= total_size) {
            size_t remaining = block->size - total_size;

            if (remaining >= sizeof(kblock_t) + sizeof(uint32_t) + 8) {
                size_t old_size = block->size;

                block->size = total_size;
                block->allocated = true;
                block->file = file;
                block->line = line;
                write_canary(block);

                kblock_t* new_block = (kblock_t*)((uint8_t*)block + block->size);
                new_block->size = old_size - total_size;
                new_block->allocated = false;
                new_block->file = "free";
                new_block->line = 0;
                write_canary(new_block);
            } else {
                block->allocated = true;
                block->file = file;
                block->line = line;
                write_canary(block);
            }

            void* payload = (void*)((uint8_t*)block + sizeof(kblock_t));

            if (!heap_check("after kmalloc")) {
                log_error("heap corrupted immediately after kmalloc at %s:%d\n", file, line);
                heap_dump();
            }

            return payload;
        }

        kblock_t* next = get_next_block(block);
        if (!next) {
            break;
        }

        block = next;
    }

    log_error("kmalloc failed: size=%d total=%d at %s:%d\n",
              size, total_size, file, line);
    return NULL;
}

void* kzmalloc_debug(size_t size, const char* file, int line) {
    void* ptr = kmalloc_debug(size, file, line);

    if (!ptr) {
        return NULL;
    }

    memset(ptr, 0, size);
    return ptr;
}

void kfree_debug(void* ptr, const char* file, int line) {
    log_debug("Free called at %s:%d for addr %x\n", file, line, ptr); 

    num_frees++; 
    if (!ptr) {
        return;
    }

    if (!ptr_in_heap(ptr)) {
        log_error("kfree bad pointer outside heap: ptr=%x at %s:%d\n",
                  ptr, file, line);
        return;
    }

    kblock_t* block = (kblock_t*)((uint8_t*)ptr - sizeof(kblock_t));

    if (!valid_block_basic(block)) {
        log_error("kfree invalid block for ptr=%x at %s:%d\n", ptr, file, line);
        heap_dump();
        return;
    }

    if (!block->allocated) {
        log_error("double free? ptr=%x block=%x originally_at=%s:%d free_called_at=%s:%d\n",
                  ptr, block, block->file, block->line, file, line);
        heap_dump();
        return;
    }

    if (!check_canary(block)) {
        log_error("heap overflow detected on free!\n");
        log_error("ptr=%x block=%x allocated_at=%s:%d freed_at=%s:%d canary=%x expected=%x\n",
                  ptr,
                  block,
                  block->file,
                  block->line,
                  file,
                  line,
                  *block_canary(block),
                  KHEAP_CANARY);
        heap_dump();
        return;
    }

    block->allocated = false;
    block->file = "free";
    block->line = 0;

    coalesce(block);
    write_canary(block);
    
    if (!heap_check("after kfree")) {
        log_error("heap corrupted immediately after kfree at %s:%d\n", file, line);
        heap_dump();
    }
}
