#include "compositor.h"

static int32_t compositor_get_events(int32_t input_fd, input_event_t* event_buffer) {
    int32_t bytes_read = read(input_fd, (char*)event_buffer, 32 * sizeof(input_event_t));
   
    if (bytes_read < 0)
        return -1;

    return bytes_read / sizeof(input_event_t);
}

static void compositor_handle_event(input_event_t* event) {
    if (event->type == INPUT_EVENT_KEY) {
        printf("%d\n", event->modifiers);
    } else if (event->type == INPUT_EVENT_MOUSE) {

    } else if (event->type == INPUT_EVENT_MOUSE_CLICK) {

    }
}

static void compositor_windows_init() {
    for (uint32_t i = 0; i < MAX_WINDOWS; i++) {
        windows[i].id = i;
    }
}

int main() {
    close(1); // close stdout
    int32_t serial_fd = open("/dev/serial", O_WRONLY, 0); // open serial for output
    if (serial_fd < 0) {
        return -1; 
    }

    gfx_context_t* ctx = gfx_get_context(NULL, NULL);
    if (!ctx) {
        return -1;
    }
    
    errno = 0;
    int32_t input_fd = open("/dev/input", O_RDONLY, 0);
    if (input_fd < 0) {
        perror("open");
        return -1;
    }

    compositor_windows_init();

    uint32_t child_pid = fork();
    if (child_pid == 0) {
        errno = 0; 
         
        int ret = execve("/bin/tty", NULL);
        
        if (ret == -1) {
            perror("execve");
            exit(-1);
        }
    }

    while (1) {
        input_event_t events[32];
        int32_t num_events = compositor_get_events(input_fd, events);
        if (num_events <= 0) {
            yield();
            continue;
        }
        
        for (uint32_t i = 0; i < num_events; i++) { 
            if (i >= 32)
                break;
            
            input_event_t event = events[i];
            
            compositor_handle_event(&event); 
        }
        
        window_blit(ctx, 0); // hardcode for now, eventually loop
        gfx_flush(ctx);
    }

    gfx_free_context(ctx);
}
