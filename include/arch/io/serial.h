#ifndef INCLUDE_ARCH_IO_SERIAL_H_
#define INCLUDE_ARCH_IO_SERIAL_H_

#if defined ARCH_riscv64
    #define COM1 0x10000000
#elif defined ARCH_i686
    #define COM1 0x3F8
#endif

#endif
