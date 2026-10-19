#include "boot/dtb.h"
#include "memory/heap.h"
#include "memory/mmio.h"
#include "interrupts/timer.h"
#include <log.h>
#include <string.h>

static inline uint32_t be32_to_cpu(uint32_t x) {
    return __builtin_bswap32(x);
}

static inline uint64_t be64_to_cpu(uint64_t x) {
    return __builtin_bswap64(x);
}

static uint64_t dtb_read_uint(uint8_t* value, uint32_t len) {
    uint64_t result = 0;

    for (uint32_t i = 0; i < len / 4; ++i) {
        result <<= 32;
        result |= be32_to_cpu(*(uint32_t*)value);
        value += 4;
    }

    return result;
}

static inline uint8_t* align_to_4(uint8_t* ptr) {
    return (uint8_t*)(((uintptr_t)ptr + 3) & ~3ULL);
}

static dtb_struct_node_t* dtb_stack_top(dtb_token_stack_t* stack) {
    if (stack->size == 0)
        return NULL;

    return &stack->stack[stack->size - 1];;
}

static inline void dtb_stack_push(dtb_token_stack_t* stack, dtb_struct_node_t node) {
    stack->stack[stack->size++] = node;
}

static dtb_struct_node_t dtb_stack_pop(dtb_token_stack_t* stack) {
    dtb_struct_node_t token = { 0 };
    if (stack->size == 0)
        return token;

    token = stack->stack[stack->size - 1];
    stack->size--;
    return token;
}

static uint8_t dtb_compatible_contains(uint8_t* value, uint32_t len, const char* target) {
    uint32_t offset = 0;
    while (offset < len) {
        char* compatible = (char*)(value + offset);
        if (strcmp(compatible, target) == 0)
            return 1;

        offset += strlen(compatible) + 1;
    }

    return 0;
}

uint8_t dtb_init(dtb_tree_t* dtb) {
    // DTB is in big endian, we must convert to little endian
    dtb->magic = be32_to_cpu(dtb->magic);
    dtb->totalsize = be32_to_cpu(dtb->totalsize);
    dtb->off_dt_struct = be32_to_cpu(dtb->off_dt_struct);
    dtb->off_dt_strings = be32_to_cpu(dtb->off_dt_strings);
    dtb->off_mem_rsvmap = be32_to_cpu(dtb->off_mem_rsvmap);
    dtb->version = be32_to_cpu(dtb->version);
    dtb->last_comp_version = be32_to_cpu(dtb->last_comp_version);
    dtb->boot_cpuid_phys = be32_to_cpu(dtb->boot_cpuid_phys);
    dtb->size_dt_strings = be32_to_cpu(dtb->size_dt_strings);
    dtb->size_dt_struct = be32_to_cpu(dtb->size_dt_struct);

    if (dtb->magic != DTB_MAGIC)
        return 0;

    return 1;
}

uint8_t dtb_parse_struct(dtb_tree_t* dtb, kernel_boot_info_t* info) {
    uint8_t* struct_ptr = (uint8_t*)dtb + dtb->off_dt_struct;
    dtb_token_stack_t token_stack = { 0 };

    while (1) {
        uint32_t token = be32_to_cpu(*(uint32_t*)struct_ptr);
        struct_ptr += 4;

        switch (token) {
            case FDT_BEGIN_NODE: {
                char* node_name = (char*)struct_ptr;
                while (*struct_ptr != 0)
                    struct_ptr++;

                struct_ptr = align_to_4(++struct_ptr); // align to 4 bytes and skip null terminator
            
                dtb_struct_node_t node = {
                    .name = node_name,
                    .address_cells = 2,
                    .size_cells = 1,
                };

                dtb_stack_push(&token_stack, node);
                break;
            }
            
            case FDT_END_NODE: {
                if (token_stack.size == 0)
                    return 0;

                dtb_struct_node_t* node = dtb_stack_top(&token_stack);
                if (node->seen_reg) {
                    if (node->type == DTB_UART) {
                        riscv_mmio_info[MMIO_UART0_IDX].addr = node->reg_addr;
                        riscv_mmio_info[MMIO_UART0_IDX].size = node->reg_size;
                    }

                    if (node->type == DTB_PLIC) {
                        riscv_mmio_info[MMIO_PLIC_IDX].addr = node->reg_addr;
                        riscv_mmio_info[MMIO_PLIC_IDX].size = node->reg_size;
                    }

                    if (node->type == DTB_CLINT) {
                        riscv_mmio_info[MMIO_CLINT_IDX].addr = node->reg_addr;
                        riscv_mmio_info[MMIO_CLINT_IDX].size = node->reg_size;
                    }
                }

                if (node->type == DTB_INITRD && node->reg_addr && node->reg_end && (node->reg_end > node->reg_addr)) {
                    info->regions[info->region_count].mem_addr = node->reg_addr;
                    info->regions[info->region_count].mem_size = (size_t)(node->reg_end - node->reg_addr);
                    info->regions[info->region_count].type = MEM_FILESYSTEM;

                    info->region_count++;

                    log_debug("%lx\n", info->regions[info->region_count].mem_addr);
                    log_debug("%l\n", info->regions[info->region_count].mem_size);
                }

                dtb_stack_pop(&token_stack);
                break;
            }

            case FDT_PROP: {
                if (token_stack.size == 0)
                    return 0;

                dtb_struct_node_t* node = dtb_stack_top(&token_stack);
                dtb_struct_node_t parent = token_stack.stack[token_stack.size - 2];

                uint32_t len = be32_to_cpu(*(uint32_t*)struct_ptr);
                struct_ptr += 4;

                uint32_t nameoff = be32_to_cpu(*(uint32_t*)struct_ptr);
                if (nameoff >= dtb->size_dt_strings)
                    return 0;

                struct_ptr += 4;
                char* name = (char*)((uint8_t*)dtb + dtb->off_dt_strings + nameoff);

                uint8_t* value = (uint8_t*)struct_ptr;
                if (strcmp(name, "#address-cells") == 0) {
                    node->address_cells = be32_to_cpu(*(uint32_t*)value);
                } else if (strcmp(name, "#size-cells") == 0) {
                    node->size_cells = be32_to_cpu(*(uint32_t*)value);
                } else if (strcmp(name, "device_type") == 0) {
                    if (strcmp((char*)value, "memory") == 0)
                        node->type = DTB_RAM;
                } else if (strcmp(name, "compatible") == 0) {
                    if (dtb_compatible_contains(value, len, "ns16550a"))
                        node->type = DTB_UART;
                    else if (dtb_compatible_contains(value, len, "sifive,plic-1.0.0"))
                        node->type = DTB_PLIC;
                    else if (dtb_compatible_contains(value, len, "sifive,clint0"))
                        node->type = DTB_PLIC;
                } else if (node->name && strcmp(node->name, "chosen") == 0) {
                    if (strcmp(name, "linux,initrd-start") == 0) {
                        node->type = DTB_INITRD;
                        node->reg_addr = dtb_read_uint(value, len);
                    } else if (strcmp(name, "linux,initrd-end") == 0) {
                        node->type = DTB_INITRD;
                        node->reg_end = dtb_read_uint(value, len);
                    }
                } else if (strcmp(name, "reg") == 0) {
                    if (len % ((parent.address_cells + parent.size_cells) * 4) != 0)
                        return 0;
                    
                    for (uint32_t bytes_read = 0; bytes_read < len; bytes_read += (parent.address_cells + parent.size_cells) * 4) {
                        uint64_t address = 0; 
                        for (uint32_t i = 0; i < parent.address_cells; ++i) {
                            address <<= 32;
                            address |= be32_to_cpu(*(uint32_t*)value);
                            value += 4;
                        }

                        uint64_t size = 0; 
                        for (uint32_t i = 0; i < parent.size_cells; ++i) {
                            size <<= 32;
                            size |= be32_to_cpu(*(uint32_t*)value);
                            value += 4;
                        }
                        
                        node->seen_reg = 1;
                        node->reg_addr = address;
                        node->reg_size = size;
                        
                        memory_type_t type;
                        if (node->type == DTB_RAM) {
                            type = MEM_USABLE;

                            riscv_mmio_info[MMIO_RAM_IDX].addr = (uintptr_t)address;
                            riscv_mmio_info[MMIO_RAM_IDX].size = (size_t)size;
                        } else if (parent.name && strcmp(parent.name, "reserved-memory") == 0) {
                            type = MEM_RESERVED;
                        } else if (node->type == DTB_UART) {
                            riscv_mmio_info[MMIO_UART0_IDX].addr = (uintptr_t)address;
                            riscv_mmio_info[MMIO_UART0_IDX].size = (uintptr_t)size;
                            continue;
                        } else if (node->type == DTB_PLIC) {
                            riscv_mmio_info[MMIO_PLIC_IDX].addr = (uintptr_t)address;
                            riscv_mmio_info[MMIO_PLIC_IDX].size = (uintptr_t)size;
                            continue;
                        } else if (node->type == DTB_CLINT) {
                            riscv_mmio_info[MMIO_CLINT_IDX].addr = (uintptr_t)address;
                            riscv_mmio_info[MMIO_CLINT_IDX].size = (uintptr_t)size;
                            continue;
                        } else {
                            continue;
                        }

                        memory_region_t region = {
                            .mem_addr = (uintptr_t)address,
                            .mem_size = (size_t)size,
                            .type = type,
                        };

                        if (info->region_count >= MAX_MEMORY_REGIONS) {
                            log_error("ran out of room for the memory regions\n");
                            return 0;
                        }

                        info->regions[info->region_count++] = region;
                    }
                } else if (strcmp(name, "timebase-frequency") == 0) {
                    timebase_frequency = be32_to_cpu(*(uint32_t*)value);
                    info->timer_frequency = timebase_frequency;
                    log_debug("timebase-frequency=%l\n", timebase_frequency);
                }

                struct_ptr += len;
                struct_ptr = align_to_4(struct_ptr);

                break;
            } 
            case FDT_NOP: break;
            case FDT_END: return 1;
            default: log_error("received unknow DTB token\n"); return 0;
        }
    }

    return 1;
}
