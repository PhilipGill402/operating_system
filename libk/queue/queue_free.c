#include <queue.h>

void queue_free(queue_t* queue){
    kfree(queue->array);
    queue->array = NULL;
    queue->size = 0;
    queue->capacity = 0;
    queue->head = 0;
    queue->rear = 0;
    queue->element_size = 0;
}
