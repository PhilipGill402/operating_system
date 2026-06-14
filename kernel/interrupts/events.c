#include "interrupts/events.h"

queue_t input_buffer;

void events_init() {
    input_buffer = queue_create(sizeof(input_event_t));
}
