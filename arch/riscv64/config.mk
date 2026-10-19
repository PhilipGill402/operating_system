TARGET := riscv64-elf

QEMU_ARCH := riscv64

ARCH_CFLAGS += -march=rv64gc -mabi=lp64d -mcmodel=medany

# OpenSBI boots the kernel directly rather than through a GRUB ISO.
BOOT_MODE := opensbi

# QEMU can load our ELF directly.
RUN_DEPS := $(KERNEL_ELF) $(INITRD_IMG)

LIBC_FLAGS := -march=rv64gc -mabi=lp64d -ffreestanding

QEMU_ARGS = \
	-machine virt \
	-m 128M \
	-bios default \
	-kernel $(KERNEL_ELF) \
	-nographic \
	-initrd $(INITRD_IMG) 
