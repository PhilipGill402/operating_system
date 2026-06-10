#include <unistd.h>

#include <stdio.h>

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

    if (ret < 0) {
        errno = -ret;
        return -1;
    }

    return ret;
}

int32_t __sys6(uint32_t sys_num, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5, uint32_t arg6) {
    int32_t ret; 
    register uint32_t r_eax asm("eax") = sys_num;
    register uint32_t r_ebx asm("ebx") = arg1;
    register uint32_t r_ecx asm("ecx") = arg2;
    register uint32_t r_edx asm("edx") = arg3;
    register uint32_t r_esi asm("esi") = arg4;
    register uint32_t r_edi asm("edi") = arg5;
    register uint32_t r_ebp asm("ebp") = arg6;

    asm volatile(
        "int $0x80"
        : "+a"(r_eax)
        : "b"(r_ebx),
          "c"(r_ecx),
          "d"(r_edx),
          "S"(r_esi),
          "D"(r_edi),
          "r"(r_ebp)
        : "memory"
    );

    ret = (int32_t)r_eax;

    if (ret < 0) {
        errno = -ret;
        return -1;
    }

    return ret;
}

