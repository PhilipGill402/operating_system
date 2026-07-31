#include "io/framebuffer.h"

framebuffer_t framebuffer;
fb_shared_buffer_t fb_shared_buffer;

uint32_t framebuffer_set_pixel_raw(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= framebuffer.width || y >= framebuffer.height)
        return 0;
    
    uint32_t* pixel = (uint32_t*)(framebuffer.addr + y * framebuffer.pitch + x * sizeof(uint32_t));
    *pixel = color; 
    
    return 1;
}

void framebuffer_clear(uint32_t color) {
    for (uint32_t x = 0; x < framebuffer.width; x++) {
        for (uint32_t y = 0; y < framebuffer.height; y++) {
            framebuffer_set_pixel_raw(x, y, color);
        }
    }
}

static int32_t framebuffer_shared_buffer_init() {
    fb_shared_buffer.width = framebuffer.width;
    fb_shared_buffer.height = framebuffer.height;
    fb_shared_buffer.pitch = framebuffer.width * sizeof(uint32_t);
    fb_shared_buffer.bpp = framebuffer.bpp;
    fb_shared_buffer.size = fb_shared_buffer.pitch * framebuffer.height;
    fb_shared_buffer.frame_count = PAGE_ALIGN_UP(fb_shared_buffer.size) / PAGE_SIZE;
    fb_shared_buffer.kernel_vaddr = FB_SHARED_KERNEL_VADDR;
    fb_shared_buffer.owner_pid = -1;
    fb_shared_buffer.frames = kmalloc(fb_shared_buffer.frame_count * sizeof(uint32_t));

    if (!fb_shared_buffer.frames)
        return -1;

    for (uint32_t i = 0; i < fb_shared_buffer.frame_count; i++) {
        uint32_t frame = pmm_alloc_frame();

        if (!frame)
            return -1;

        fb_shared_buffer.frames[i] = frame;

        uint32_t* page = (uint32_t*)FB_SHARED_KERNEL_VADDR + i * PAGE_SIZE;
        arch_page_map(arch_kernel_address_space(), page, frame, ARCH_PAGE_WRITE);
        memset(page, 0, PAGE_SIZE);
    }

    framebuffer.backbuffer = (uint32_t*)FB_SHARED_KERNEL_VADDR;

    return 1;
}

int32_t framebuffer_init(multiboot_info_t* mbi) {
    uint64_t fb_phys = mbi->framebuffer_addr;
    
    if (fb_phys > 0xFFFFFFFF) {
        log_error("framebuffer above 4 GB not supported\n");
        return 0;
    }

    uint32_t fb_size = mbi->framebuffer_pitch * mbi->framebuffer_height;

    for (uint32_t offset = 0; offset < fb_size; offset += PAGE_SIZE) {
        arch_page_map(arch_kernel_address_space(), FRAMEBUFFER_VIRT + offset, fb_phys + offset, ARCH_PAGE_PRESENT | ARCH_PAGE_WRITE);
    }

    framebuffer.addr = (uint8_t*)FRAMEBUFFER_VIRT;
    framebuffer.width = mbi->framebuffer_width;
    framebuffer.height = mbi->framebuffer_height;
    framebuffer.pitch = mbi->framebuffer_pitch;
    framebuffer.bpp = mbi->framebuffer_bpp;
    
    if (!framebuffer_shared_buffer_init())
        return -1;
        
    framebuffer_clear(FB_BLACK);

    return 1;
}

