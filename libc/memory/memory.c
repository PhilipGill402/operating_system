#include <memory.h>

int memcmp(const void* s1, const void* s2, size_t n) {
    const unsigned char* a = (const unsigned char*) s1;
    const unsigned char* b = (const unsigned char*) s2;

    for (size_t i = 0; i < n; i++) {
        if (a[i] != b[i]) {
            return a[i] - b[i]; 
        }
    }

    return 0;
}

void* memcpy(void* restrict dstptr, const void* restrict srcptr, size_t size) {
    unsigned char* dst = (unsigned char*) dstptr;
    const unsigned char* src = (unsigned char*) srcptr;

    for (size_t i = 0; i < size; i++) {
        dst[i] = src[i];
    }

    return dstptr;
}

void* memmove(void* dst, const void* src, size_t size) {
    unsigned char* d = (unsigned char*) dst;
    const unsigned char* s = (unsigned char*) src;

    if (d < s) {
        for (size_t i = 0; i < size; i++) {
            d[i] = s[i];
        }
    } else {
        for (size_t i = size; i != 0; i--) {
            d[i-1] = s[i-1];
        }
    }

    return dst;
}

void* memset(void* buffer, int value, size_t size) {
    unsigned char* buf = (unsigned char*) buffer;

    for (size_t i = 0; i < size; i++) {
        buf[i] = (unsigned char) value;
    }

    return buf;
}
