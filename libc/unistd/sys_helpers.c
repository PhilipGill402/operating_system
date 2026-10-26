#include <unistd.h>

int32_t __sys0(uint64_t sys_num) {
    int32_t ret;
    asm volatile (
        "ecall"
        : "+r"(ret)
        : "r"(sys_num)
        : "memory"
    );
    
    // TODO: set errno
    if (ret < 0) {
        errno = -ret; 
        return -1;
    }

    return ret;
}

int32_t __sys1(uint64_t sys_num, uint64_t arg1) {
    int32_t ret;
    asm volatile (
        "ecall"
        : "+r"(ret)
        : "r"(sys_num), "r"(arg1)
        : "memory"
    );

    // TODO: set errno
    if (ret < 0) {
        errno = -ret;
        return -1;
    }

    return ret;
}

int32_t __sys2(uint64_t sys_num, uint64_t arg1, uint64_t arg2) {
    int32_t ret;
    asm volatile (
        "ecall"
        : "+r"(ret)
        : "r"(sys_num), "r"(arg1), "r"(arg2)
        : "memory"
    );

    if (ret < 0) {
        errno = -ret;
        return -1;
    }

    return ret;
}

int32_t __sys3(uint64_t sys_num, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    register uint64_t a0 asm("a0") = sys_num;
    register uint64_t a1 asm("a1") = arg1;
    register uint64_t a2 asm("a2") = arg2;
    register uint64_t a3 asm("a3") = arg3;

    asm volatile(
        "ecall"
        : "+r"(a0)
        : "r"(a1), "r"(a2), "r"(a3)
        : "memory"
    );

    int32_t ret = (int32_t)a0;

    if (ret < 0) {
        errno = -ret;
        return -1;
    }

    return ret;
}

int32_t __sys4(uint64_t sys_num, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4) {
    int32_t ret;

    asm volatile(
        "ecall"
        : "+r"(ret)
        : "r"(sys_num),
          "r"(arg1),
          "r"(arg2),
          "r"(arg3),
          "r"(arg4)
        : "memory"
    );

    if (ret < 0) {
        errno = -ret;
        return -1;
    }

    return ret;
}

int32_t __sys6(uint64_t sys_num, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6) {
    int32_t ret; 
    register uint64_t r_eax asm("a0") = sys_num;
    register uint64_t r_ebx asm("a1") = arg1;
    register uint64_t r_ecx asm("a2") = arg2;
    register uint64_t r_edx asm("a3") = arg3;
    register uint64_t r_esi asm("a4") = arg4;
    register uint64_t r_edi asm("a5") = arg5;
    register uint64_t r_ebp asm("a6") = arg6;

    asm volatile(
        "ecall"
        : "+r"(r_eax)
        : "r"(r_ebx),
          "r"(r_ecx),
          "r"(r_edx),
          "r"(r_esi),
          "r"(r_edi),
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

