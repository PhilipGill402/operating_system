#include <queue.h>

void* queue_first(const queue_t* queue){
    void* val = (char*)queue->array + queue->head * queue->element_size;
    return val;
}
