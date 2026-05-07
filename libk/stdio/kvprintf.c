#include <stdio.h>

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
