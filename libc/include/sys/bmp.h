#ifndef LIBC_INCLUDE_SYS_BMP_H_
#define LIBC_INCLUDE_SYS_BMP_H_

#include <stdint.h>

typedef struct bmp_file_header {
    uint16_t signature;      // "BM" = 0x4D42
    uint32_t file_size;      // total file size in bytes
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t pixel_offset;   // byte offset where pixel data starts
} __attribute__((packed)) bmp_file_header_t;

typedef struct bmp_info_header {
    uint32_t header_size;      // usually 40
    int32_t  width;
    int32_t  height;
    uint16_t planes;           // usually 1
    uint16_t bits_per_pixel;   // often 24 or 32
    uint32_t compression;      // 0 = uncompressed
    uint32_t image_size;
    int32_t  x_pixels_per_meter;
    int32_t  y_pixels_per_meter;
    uint32_t colors_used;
    uint32_t important_colors;
} __attribute__((packed)) bmp_info_header_t;

#endif // !LIBC_INCLUDE_SYS_BMP_H_
