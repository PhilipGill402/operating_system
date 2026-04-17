#include <queue.h>

bool queue_resize(queue_t* queue, int new_capacity){
    void* new_array;
    new_array = kmalloc(queue->element_size * new_capacity);

    if (new_array == NULL){
        return false;
    }
    
    for (int i = 0; i < queue->size; i++){
        int index = (queue->head + i) % queue->capacity;
        memcpy((char*)new_array + i * queue->element_size, (char*)queue->array + index * queue->element_size, queue->element_size);
    }
    
    kfree(queue->array);

    queue->array = new_array;
    queue->head = 0;
    queue->rear = queue->size;
    queue->capacity = new_capacity;

    return true;
}
