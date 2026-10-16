#ifndef LIBK_INCLUDE_HELPERS_H_
#define LIBK_INCLUDE_HELPERS_H_

#define ALIGN_UP(x, a) (((x) + (a) - 1) & ~((a) - 1))
#define ALIGN_DOWN(x, a) ((x) & ~((a) - 1))
#define DIV_CEIL(x, y) (((x) + (y) - 1) / (y))

#endif
