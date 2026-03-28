#include "tty.h"

static input_buffer_t keyboard_buffer;

static const char* commands[] = {
    "help",
    "echo <text>",
    "clear",
    "ticks",
    "mem",
    "allocpage",
    "pagefault",
    "exit"
};

static uint8_t num_commands = 8;

void input_buffer_reset() {
    keyboard_buffer.data[0] = '\0';
    keyboard_buffer.length = 0;
    keyboard_buffer.ready = 0;
}

void input_buffer_push(char c) {
    if (keyboard_buffer.length + 1 >= INPUT_BUFFER_SIZE) {
        return;
    }

    keyboard_buffer.data[keyboard_buffer.length++] = c;
    keyboard_buffer.data[keyboard_buffer.length] = '\0';
}
void input_buffer_pop() {
    if (keyboard_buffer.length == 0) {
        return;
    }

    keyboard_buffer.data[--keyboard_buffer.length] = '\0';
}

void input_buffer_submit() {
    keyboard_buffer.data[keyboard_buffer.length] = '\0';
    keyboard_buffer.ready = 1;
}

void print_memory_info() {
    printf("Memory Info\n");
    printf("-----------\n");
    printf("Page size: %d bytes\n", PAGE_SIZE);
    printf("Total frames: %d\n", pmm.total_frames);
    printf("Used frames: %d\n", pmm.used_frames);
    printf("Free frames: %d\n", pmm.free_frames);
    printf("Bitmap bytes: %d\n\n", pmm.bitmap_bytes);

    printf("Heap start: %x\n", KHEAP_START);
    printf("Heap end: %x\n", heap_end);
    printf("Heap current: %x\n", heap_curr);
}

void read_command(char* line) {
    char* cmd = strtok(line, ' ');

    if (strcmp(cmd, "echo") == 0) {
        cmd = strtok(NULL, ' '); 
        do {
            printf("%s ", cmd);
            cmd = strtok(NULL, ' ');
        } while (cmd);
        printf("\n");
    } else if (strcmp(cmd, "exit") == 0) { 
        return;
    } else if (strcmp(cmd, "clear") == 0) {
        terminal_initialize(); 
    } else if (strcmp(cmd, "ticks") == 0) {
        printf("%d\n", timer_get_ticks());
    } else if (strcmp(cmd, "mem") == 0) {
        print_memory_info();
    } else {
        for (uint8_t i = 0; i < num_commands; i++) {
            printf("%s\n", commands[i]);
        }         
    }

}

void tty() {
    input_buffer_reset();
    char cmd[INPUT_BUFFER_SIZE] = "";

    while (strcmp(cmd, "exit") != 0) {
        printf(">> ");

        while (!keyboard_buffer.ready) {
            __asm__ __volatile__("hlt");
        }
        
        strcpy(cmd, keyboard_buffer.data);
        read_command(cmd); 
    
        input_buffer_reset();
    } 
}
