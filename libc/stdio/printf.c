#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    char* str;
    size_t size;
    size_t capacity;
} sprintf_c_ctx_t;

typedef struct {
    string_t* str;
    size_t size;
    size_t capacity;
} sprintf_s_ctx_t;

static void fprintf_putc(char c, void* ctx) {
    if (!ctx) return; 

    uint32_t fd = *(uint32_t*)ctx;

    write(fd, &c, 1);
}

static void snprintf_c_putc(char c, void* ctx) {
    sprintf_c_ctx_t* snprintf_ctx = (sprintf_c_ctx_t*)ctx;

    if (!snprintf_ctx || !snprintf_ctx->str) return;
    
    if (snprintf_ctx->size + 1 < snprintf_ctx->capacity) {
        snprintf_ctx->str[snprintf_ctx->size++] = c;
    }
}

static void sprintf_c_putc(char c, void* ctx) {
    sprintf_c_ctx_t* sprintf_ctx = (sprintf_c_ctx_t*)ctx;

    if (!sprintf_ctx || !sprintf_ctx->str) return;

    sprintf_ctx->str[sprintf_ctx->size++] = c;
}

static void sprintf_s_putc(char c, void* ctx) {
    sprintf_s_ctx_t* sprintf_ctx = (sprintf_s_ctx_t*)ctx;

    if (!sprintf_ctx || !sprintf_ctx->str) return;
    
    string_append_chr(sprintf_ctx->str, c); 
}

static void snprintf_s_putc(char c, void* ctx) {
    sprintf_s_ctx_t* sprintf_ctx = (sprintf_s_ctx_t*)ctx;

    if (!sprintf_ctx || !sprintf_ctx->str) return;
    
    if (sprintf_ctx->size >= sprintf_ctx->capacity) {
        string_append_chr(sprintf_ctx->str, c);
        sprintf_ctx->size++;
    }
}

static void putc(char c, void* ctx) {
    (void)ctx;
	
    write(stdout, &c, 1);
}

static void uint_to_str(uint32_t num, char* str) {
    int i = 0;

    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    while (num > 0) {
        str[i++] = (num % 10) + '0';
        num /= 10;
    }

    str[i] = '\0';

    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}

static void int_to_str(int num, char* str) {
    unsigned int value;
    int i = 0;
    int negative = 0;

    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    if (num < 0) {
        negative = 1;
        value = (unsigned int)(-(num + 1)) + 1;
    } else {
        value = (unsigned int)num;
    }

    while (value > 0) {
        str[i++] = (value % 10) + '0';
        value /= 10;
    }

    if (negative) {
        str[i++] = '-';
    }

    str[i] = '\0';

    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}

static void uint32_to_hex(uint32_t num, char* str) {
    const char hex_digits[] = "0123456789ABCDEF";

    str[0] = '0';
    str[1] = 'x';

    for (int i = 0; i < 8; i++) {
        uint8_t digit = (num >> (28 - 4 * i)) & 0xF;
        str[2 + i] = hex_digits[digit];
    }

    str[10] = '\0';
}

int kvprintf(void (*putc)(char c, void* ctx), void* ctx, const char* format, va_list args) {
    int written = 0;

    while (*format != '\0') {
        size_t maxrem = INT_MAX - written;

        if (format[0] != '%' || format[1] == '%') {
            if (format[0] == '%')
                format++;

            size_t amount = 1;

            while (format[amount] && format[amount] != '%')
                amount++;

            if (maxrem < amount) {
                return -1;
            }

            for (size_t i = 0; i < amount; i++) {
                putc(format[i], ctx);
            }

            format += amount;
            written += amount;
            continue;
        }

        const char* format_begun_at = format++;

        if (*format == 'c') {
            format++;

            char c = (char) va_arg(args, int);

            if (!maxrem) {
                return -1;
            }

            putc(c, ctx);
            written++;
        }

        else if (*format == 's') {
            format++;

            const char* str = va_arg(args, const char*);

            if (!str) {
                str = "(null)";
            }

            size_t len = strlen(str);

            if (maxrem < len) {
                return -1;
            }

            for (size_t i = 0; i < len; i++) {
                putc(str[i], ctx);
            }

            written += len;
        }

        else if (*format == 'd') {
            format++;

            int num = va_arg(args, int);
            char str[12];

            int_to_str(num, str);

            size_t len = strlen(str);

            if (maxrem < len) {
                return -1;
            }

            for (size_t i = 0; i < len; i++) {
                putc(str[i], ctx);
            }

            written += len;
        }

        else if (*format == 'x') {
            format++;

            unsigned int hex = va_arg(args, unsigned int);
            char str[11];

            uint32_to_hex((uint32_t) hex, str);

            size_t len = strlen(str);

            if (maxrem < len) {
                return -1;
            }

            for (size_t i = 0; i < len; i++) {
                putc(str[i], ctx);
            }

            written += len;
        }

        else if (*format == 'u') {
            format++;

            uint32_t num= va_arg(args, uint32_t);
            char str[20];

            uint_to_str(num, str);

            size_t len = strlen(str);

            if (maxrem < len) return -1;

            for (size_t i = 0; i < len; i++) {
                putc(str[i], ctx);
            }

            written += len;
        }

        else if (*format == 'r') {
            format++;
            string_t string = va_arg(args, string_t);
            
            for (int i = 0; i < string_len(&string); i++) {
                putc(string.str[i], ctx);
            }

            written += string_len(&string);
        }

        else {
            format = format_begun_at;

            size_t len = strlen(format);

            if (maxrem < len) {
                return -1;
            }

            for (size_t i = 0; i < len; i++) {
                putc(format[i], ctx);
            }

            written += len;
            format += len;
        }
    }

    return written;
}

int printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int written = kvprintf(putc, NULL, fmt, args);
    va_end(args);

    return written;
}

int sprintf_c(char* str, const char* fmt, ...) {
    sprintf_c_ctx_t ctx = {
        .str = str,
        .size = 0,
        .capacity = 0 
    };

    va_list args;
    va_start(args, fmt);
    int written = kvprintf(sprintf_c_putc, &ctx, fmt, args);
    va_end(args);
    
    ctx.str[ctx.size] = '\0';

    return written;
}

int snprintf_c(char* str, size_t size, const char* fmt, ...) {
    sprintf_c_ctx_t ctx = {
        .str = str,
        .size = 0,
        .capacity = size
    };

    va_list args;
    va_start(args, fmt);
    int written = kvprintf(snprintf_c_putc, &ctx, fmt, args);
    va_end(args);
    
    if (size > 0) {
        ctx.str[ctx.size] = '\0';
    }

    return written;
}

int sprintf_s(string_t* str, const char* fmt, ...) {
    sprintf_s_ctx_t ctx = {
        .str = str,
        .size = 0,
        .capacity = 0 
    };

    va_list args;
    va_start(args, fmt);
    int written = kvprintf(sprintf_s_putc, &ctx, fmt, args);
    va_end(args);
    
    return written;
}

int snprintf_s(string_t* str, size_t size, const char* fmt, ...) {
    sprintf_s_ctx_t ctx = {
        .str = str,
        .size = 0,
        .capacity = size
    };

    va_list args;
    va_start(args, fmt);
    int written = kvprintf(snprintf_s_putc, &ctx, fmt, args);
    va_end(args);
    
    return written;
}

int fprintf(uint32_t fd, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int written = kvprintf(fprintf_putc, &fd, fmt, args);
    va_end(args);

    return written;
}


