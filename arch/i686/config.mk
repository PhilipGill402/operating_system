# Toolchain
TARGET := i686-elf

# QEMU calls its 32-bit x86 emulator "i386".
QEMU_ARCH := i386

# This architecture boots through GRUB from an ISO.
BOOT_MODE := grub-iso

# make run must build the ISO first.
RUN_DEPS := $(ISO)

QEMU_ARGS = \
	-cdrom $(ISO) \
	-serial stdio \
	-monitor none

# Verify that the linked ELF contains a valid Multiboot header.
POST_LINK_CHECK = i686-elf-grub-file --is-x86-multiboot $(KERNEL_ELF)

# Keep this matched with the filename referenced by grub.cfg.
ISO_KERNEL_NAME := kernel.elf
