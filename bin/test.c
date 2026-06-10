#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>

int main(void) {
    printf("mmap test starting...\n");

    uint8_t* mem = mmap(
        NULL,
        8192,
        PROT_READ | PROT_WRITE,
        MAP_ANON,
        -1,
        0
    );

    if (!mem) {
        printf("mmap failed: returned NULL\n");
        return 1;
    }

    printf("mmap returned address: %x\n", mem);

    /*
     * Test 1: newly mapped anonymous memory should be zeroed.
     */
    if (mem[0] != 0 || mem[4096] != 0) {
        printf("FAIL: mmap memory was not zeroed\n");
        printf("mem[0]=%u mem[4096]=%u\n", mem[0], mem[4096]);
        return 1;
    }

    printf("PASS: memory starts zeroed\n");

    /*
     * Test 2: write to both mapped pages.
     */
    mem[0] = 42;
    mem[1] = 43;
    mem[4096] = 99;
    mem[8191] = 123;

    if (mem[0] != 42 ||
        mem[1] != 43 ||
        mem[4096] != 99 ||
        mem[8191] != 123) {
        printf("FAIL: mmap memory write/read failed\n");
        return 1;
    }

    printf("PASS: write/read worked across both pages\n");

    /*
     * Test 3: munmap the exact region.
     */
    if (munmap(mem, 8192) < 0) {
        printf("FAIL: munmap failed\n");
        return 1;
    }

    printf("PASS: munmap returned success\n");

    printf("mmap test complete\n");

    return 0;
}
