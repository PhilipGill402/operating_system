#include <queue.h>

void* dequeue(queue_t* queue){
    if (queue_empty(queue)){
        return NULL;
    } 
    
    void* val = (char*)queue->array + queue->head * queue->element_size;
    queue->size--;
    queue->head = (queue-> head + 1) % queue->capacity;
    
    return val;
}
