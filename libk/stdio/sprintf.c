#include <stdio.h>

int sprintf(const char* __restrict fmt, ...) {
    va_list args; 
    va_start(args, fmt);
    
    int written = vprintf(fmt, args);

    va_end(args);

    return written;
}


