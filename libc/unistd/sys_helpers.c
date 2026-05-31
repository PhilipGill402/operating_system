#include <unistd.h>

int32_t __sys0(uint32_t sys_num) {
    int32_t ret;
    asm volatile (
        "int $0x80"
        : "=a"(ret)
        : "a"(sys_num)
        : "memory"
    );
    
    // TODO: set errno
    if (ret < 0) {
        errno = -ret; 
        return -1;
    }

    return ret;
}

int32_t __sys1(uint32_t sys_num, uint32_t arg1) {
    int32_t ret;
    asm volatile (
        "int $0x80"
        : "=a"(ret)
        : "a"(sys_num), "b"(arg1)
        : "memory"
    );

    // TODO: set errno
    if (ret < 0) {
        errno = -ret;
        return -1;
    }

    return ret;
}

int32_t __sys2(uint32_t sys_num, uint32_t arg1, uint32_t arg2) {
    int32_t ret;
    asm volatile (
        "int $0x80"
        : "=a"(ret)
        : "a"(sys_num), "b"(arg1), "c"(arg2)
        : "memory"
    );

    // TODO: set errno
    if (ret < 0) {
        errno = -ret;
        return -1;
    }

    return ret;
}

int32_t __sys3(uint32_t sys_num, uint32_t arg1, uint32_t arg2, uint32_t arg3) {
    int32_t ret; 
    asm volatile(
    "int $0x80"
    : "=a"(ret)
    : "a"(sys_num), "b"(arg1), "c"(arg2), "d"(arg3)
    : "memory"
    );

    // TODO: set errno
    if (ret < 0) {
        errno = -ret;
        return -1;
    }

    return ret;
}
