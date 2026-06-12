#include "gfx.h"

gfx_context_t* gfx_get_context(fb_info_t info, uint32_t* pixels);
void gfx_free_context(gfx_context_t* ctx, uint32_t* pixels);
