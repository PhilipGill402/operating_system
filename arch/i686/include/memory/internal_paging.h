#ifndef INCLUDE_MEMORY_INTERNAL_PAGING_H_
#define INCLUDE_MEMORY_INTERNAL_PAGING_H_

#include <stdint.h>

void i686_map_boot_page(uint32_t phys_addr);
void i686_create_temp_page_table(void);
uint8_t i686_init_shared_region(void);

#endif
