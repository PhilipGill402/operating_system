#include "errno.h"

int errno = 0;

#define ERRNO_STRING_COUNT ((int)(sizeof(errno_strings) / sizeof(errno_strings[0])))

const char* strerror(int errnum) {
    if (errnum < 0)
        errnum = -errnum;

    if (errnum > ERRNO_STRING_COUNT) return "Unknown Error";

    return errno_strings[errnum];
}

