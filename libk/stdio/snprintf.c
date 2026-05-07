#include <stdio.h>

typedef struct {
    char* str;
    size_t size;
    size_t capacity;
} snprintf_ctx_t;

void snprintf_putc(char c, void* ctx) {
    snprintf_ctx_t* snprintf_ctx = (snprintf_ctx_t*)ctx;

    if (!snprintf_ctx || !snprintf_ctx->str) return;
    
    if (snprintf_ctx->size + 1 < snprintf_ctx->capacity) {
        snprintf_ctx->str[snprintf_ctx->size++] = c;
    }
}

int snprintf(char* str, size_t size, const char* fmt, ...) {
    snprintf_ctx_t ctx = {
        .str = str,
        .size = 0,
        .capacity = size
    };

    va_list args;
    va_start(args, fmt);
    int written = kvprintf(snprintf_putc, &ctx, fmt, args);
    va_end(args);
    
    if (size > 0) {
        ctx.str[ctx.size] = '\0';
    }

    return written;
}
