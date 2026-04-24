#include <stdlib.h>
#include <unistd.h>

__attribute__((__noreturn__))
void abort(void) {
    exit(-1);
    while (1) { }
    __builtin_unreachable();
}
