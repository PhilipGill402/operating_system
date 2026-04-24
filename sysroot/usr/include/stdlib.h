#ifndef STDLIB_H_
#define STDLIB_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/cdefs.h>

#define PAGE_SIZE 4096

typedef struct block_t {
    size_t size;
    bool allocated;
    //pads block to be 16 bytes
    uint8_t _pad[7];
} block_t;

__attribute__((__noreturn__))
void abort(void);
void* malloc(size_t size);
void free(void* ptr);
void* realloc(void* ptr, size_t size);

#endif // !STDLIB_H_
