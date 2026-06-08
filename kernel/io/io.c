#include "io/io.h"

void (*io_put_char)(char, void*) = NULL;
