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

queue_t queue_create(size_t element_size) {
    queue_t queue;
    
    queue.size = 0;
    queue.capacity = 10;
    queue.head = 0;
    queue.rear = 0;
    queue.element_size = element_size;
    queue.array = malloc(element_size * 10);

    return queue;
}

int queue_empty(const queue_t* queue){
    return queue->size == 0;
}

void* queue_first(const queue_t* queue){
    void* val = (char*)queue->array + queue->head * queue->element_size;
    return val;
}

void queue_free(queue_t* queue){
    free(queue->array);
    queue->array = NULL;
    queue->size = 0;
    queue->capacity = 0;
    queue->head = 0;
    queue->rear = 0;
    queue->element_size = 0;
}

bool queue_resize(queue_t* queue, int new_capacity){
    void* new_array;
    new_array = malloc(queue->element_size * new_capacity);

    if (new_array == NULL){
        return false;
    }
    
    for (int i = 0; i < queue->size; i++){
        int index = (queue->head + i) % queue->capacity;
        memcpy((char*)new_array + i * queue->element_size, (char*)queue->array + index * queue->element_size, queue->element_size);
    }
    
    free(queue->array);

    queue->array = new_array;
    queue->head = 0;
    queue->rear = queue->size;
    queue->capacity = new_capacity;

    return true;
}

int queue_size(const queue_t* queue){
    return queue->size;
}
