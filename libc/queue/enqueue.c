#include <queue.h>

int enqueue(queue_t* queue, void* val){
    if (queue->size == queue->capacity){
        if (queue_resize(queue, queue->capacity * 2) == -1){
            return -1;
        }
    } 
    
    if (queue_empty(queue)){
        queue->head = queue->rear;
    }
    
    memcpy((char*)queue->array + queue->rear * queue->element_size, val, queue->element_size);
    queue->size++;
    queue->rear = (queue->rear + 1) % queue->capacity;
    return 0;
}
