#include <queue.h>

queue_t queue_create(size_t element_size) {
    queue_t queue;
    
    queue.size = 0;
    queue.capacity = 10;
    queue.head = 0;
    queue.rear = 0;
    queue.element_size = element_size;
    queue.array = kmalloc(element_size * 10);

    return queue;
}
