#include <stdio.h>
#include <stdlib.h>

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

int main(int argc, char* argv[]) {
    if (argc < 2) {
        return -1;
    }

    char* path = argv[1];

    int img_fd = open(path, O_RDONLY, 0);
    if (img_fd == -1) {
        perror("open");
        return -1;
    }

    uint8_t* file = malloc(25000);
    if (!file) {
        printf("failed to allocate\n");
        return -1;
    }

    int bytes_read = read(img_fd, file, 25000);
    if (bytes_read == -1) {
        perror("read");
        return -1;
    }

    bmp_file_header_t* header = (bmp_file_header_t*)file;
    bmp_info_header_t* info = (bmp_info_header_t*)(file + sizeof(bmp_file_header_t));

    // make sure signature is correct 
    if (header->signature != 0x4D42)
        return -1;

    if (info->compression)
        return -1;
    
    uint8_t* bmp = file + header->pixel_offset;

    int fb_fd = open("/dev/fb", O_RDWR, 0);
    if (fb_fd == -1) {
        perror("open");
        return -1;
    }

    int x = 100;
    int y = 100;
    int fb_width = 1024; // hardcoded for now

    for (uint32_t x_off = 0; x_off < info->width; x_off++) {
        for (uint32_t y_off = 0; y_off < info->height; y_off++) {
            uint32_t bmp_offset = (y_off * info->width + x_off) * (info->bits_per_pixel / 8);

            uint8_t b = bmp[bmp_offset];
            uint8_t g = bmp[bmp_offset + 1];
            uint8_t r = bmp[bmp_offset + 2];
            uint32_t color = (r << 16) | (g << 8) | b;
            
            
            int fb_offset = ((y + y_off) * 1024 + (x+x_off)) * 4;
            lseek(fb_fd, fb_offset);
            
            int bytes_written = write(fb_fd, bmp + bmp_offset, sizeof(uint32_t));        
            if (bytes_written == -1) {
                perror("write");
                return -1;
            }
        }
    }


    
    

    return 0;
}
