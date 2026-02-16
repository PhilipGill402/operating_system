#ifndef OS_UTILS_H_
#define OS_UTILS_H_

static inline void outb(uint16_t port, uint8_t val);
static inline uint8_t inb(uint16_t port);
static inline void io_wait(void);

#endif
