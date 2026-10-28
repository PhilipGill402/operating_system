#include <unistd.h>



int32_t __sys0(uint64_t sys_num) {
    register uint64_t a0 asm("a0") = sys_num;

    asm volatile(
        "ecall"
        : "+r"(a0)
        :
        : "memory"
    );

    int32_t ret = (int32_t)a0;

    if (ret < 0) {
        errno = -ret;
        return -1;
    }

    return ret;
}

int32_t __sys1(uint64_t sys_num, uint64_t arg1) {
    register uint64_t a0 asm("a0") = sys_num;
    register uint64_t a1 asm("a1") = arg1;

    asm volatile(
        "ecall"
        : "+r"(a0)
        : "r"(a1)
        : "memory"
    );

    int32_t ret = (int32_t)a0;

    if (ret < 0) {
        errno = -ret;
        return -1;
    }

    return ret;
}

int32_t __sys2(uint64_t sys_num, uint64_t arg1, uint64_t arg2) {
    register uint64_t a0 asm("a0") = sys_num;
    register uint64_t a1 asm("a1") = arg1;
    register uint64_t a2 asm("a2") = arg2;

    asm volatile(
        "ecall"
        : "+r"(a0)
        : "r"(a1), "r"(a2)
        : "memory"
    );

    int32_t ret = (int32_t)a0;

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
    register uint64_t a0 asm("a0") = sys_num;
    register uint64_t a1 asm("a1") = arg1;
    register uint64_t a2 asm("a2") = arg2;
    register uint64_t a3 asm("a3") = arg3;
    register uint64_t a4 asm("a4") = arg4;

    asm volatile(
        "ecall"
        : "+r"(a0)
        : "r"(a1), "r"(a2), "r"(a3), "r"(a4)
        : "memory"
    );

    int32_t ret = (int32_t)a0;

    if (ret < 0) {
        errno = -ret;
        return -1;
    }

    return ret;
}

int32_t __sys6(uint64_t sys_num, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6) {
    register uint64_t a0 asm("a0") = sys_num;
    register uint64_t a1 asm("a1") = arg1;
    register uint64_t a2 asm("a2") = arg2;
    register uint64_t a3 asm("a3") = arg3;
    register uint64_t a4 asm("a4") = arg4;
    register uint64_t a5 asm("a5") = arg5;
    register uint64_t a6 asm("a6") = arg6;

    asm volatile(
        "ecall"
        : "+r"(a0)
        : "r"(a1), "r"(a2), "r"(a3),
          "r"(a4), "r"(a5), "r"(a6)
        : "memory"
    );

    int32_t ret = (int32_t)a0;

    if (ret < 0) {
        errno = -ret;
        return -1;
    }

    return ret;
}

