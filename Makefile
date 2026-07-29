# =============================================================================
# Target architecture
# =============================================================================

ARCH ?= i686
TARGET := $(ARCH)-elf

CC := $(TARGET)-gcc
AS := $(TARGET)-as
LD := $(TARGET)-gcc

# =============================================================================
# Project directories
# =============================================================================

KERNEL_DIR := kernel
KERNEL_INC_DIR := include

ARCH_DIR := arch/$(ARCH)
ARCH_INC_DIR := $(ARCH_DIR)/include
ARCH_LINKER_SCRIPT := $(ARCH_DIR)/linker.ld

BUILD_ROOT := build
BUILD_DIR := $(BUILD_ROOT)/$(ARCH)

ISO_DIR := isodir
SYSROOT := sysroot
USER_DIR := bin

# =============================================================================
# Libraries
# =============================================================================

LIBC_DIR := libc
LIBK_DIR := libk
LIBGFX_DIR := libgfx

LIBK := $(BUILD_DIR)/libk.a
LIBC := $(LIBC_DIR)/build/libc.a
CRT0 := $(LIBC_DIR)/build/crt0.o
LIBGFX := $(LIBGFX_DIR)/build/libgfx.a

LIBC_INC := $(LIBC_DIR)/include
LIBK_INC := $(LIBK_DIR)/include

SYSROOT_LIBC := $(SYSROOT)/usr/lib/libc.a
SYSROOT_CRT0 := $(SYSROOT)/usr/lib/crt0.o
SYSROOT_STAMP := $(SYSROOT)/.libc-installed

# =============================================================================
# Output files
# =============================================================================

KERNEL_BIN := kernel.bin
ISO := myos.iso
FS := initrd.img

# =============================================================================
# Compiler and linker flags
# =============================================================================

CPPFLAGS := \
	-I$(KERNEL_INC_DIR) \
	-I$(ARCH_INC_DIR) \
	-I$(LIBK_INC)

CFLAGS := \
	-std=gnu99 \
	-ffreestanding \
	-O2 \
	-Wall \
	-Wextra \
	-g

ASFLAGS :=

LDFLAGS := \
	-T $(ARCH_LINKER_SCRIPT) \
	-ffreestanding \
	-O2 \
	-nostdlib

LIBS := -lgcc

# =============================================================================
# Source discovery
# =============================================================================

KERNEL_C_SOURCES := \
	$(shell find $(KERNEL_DIR) -type f -name '*.c')

KERNEL_ASM_SOURCES := \
	$(shell find $(KERNEL_DIR) -type f -name '*.s')

ARCH_C_SOURCES := \
	$(shell find $(ARCH_DIR) -type f -name '*.c')

ARCH_ASM_SOURCES := \
	$(shell find $(ARCH_DIR) -type f -name '*.s')

# Uppercase .S files are passed through the C preprocessor.
ARCH_CPP_ASM_SOURCES := \
	$(shell find $(ARCH_DIR) -type f -name '*.S')

# =============================================================================
# Object paths
#
# Examples:
#
# kernel/kernel.c
#   -> build/i686/kernel/kernel.o
#
# arch/i686/boot/early_init.c
#   -> build/i686/arch/i686/boot/early_init.o
#
# arch/i686/boot/boot.s
#   -> build/i686/arch/i686/boot/boot.o
# =============================================================================

KERNEL_C_OBJECTS := \
	$(patsubst %.c,$(BUILD_DIR)/%.o,$(KERNEL_C_SOURCES))

KERNEL_ASM_OBJECTS := \
	$(patsubst %.s,$(BUILD_DIR)/%.o,$(KERNEL_ASM_SOURCES))

ARCH_C_OBJECTS := \
	$(patsubst %.c,$(BUILD_DIR)/%.o,$(ARCH_C_SOURCES))

ARCH_ASM_OBJECTS := \
	$(patsubst %.s,$(BUILD_DIR)/%.o,$(ARCH_ASM_SOURCES))

ARCH_CPP_ASM_OBJECTS := \
	$(patsubst %.S,$(BUILD_DIR)/%.o,$(ARCH_CPP_ASM_SOURCES))

OBJECTS := \
	$(ARCH_ASM_OBJECTS) \
	$(ARCH_CPP_ASM_OBJECTS) \
	$(ARCH_C_OBJECTS) \
	$(KERNEL_ASM_OBJECTS) \
	$(KERNEL_C_OBJECTS)

LIBC_HEADERS := \
	$(shell find $(LIBC_INC) -type f -name '*.h')

# =============================================================================
# Phony targets
# =============================================================================

.PHONY: \
	all \
	kernel \
	debug \
	run \
	iso \
	img \
	clean \
	user \
	libc \
	libk \
	libgfx

all: kernel

kernel: $(KERNEL_BIN)

debug: CPPFLAGS += -DMALLOC_DEBUG
debug: kernel

# =============================================================================
# Kernel link
# =============================================================================

$(KERNEL_BIN): $(ARCH_LINKER_SCRIPT) $(OBJECTS) $(LIBK)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBK) $(LIBS)
	grub-file --is-x86-multiboot $@

# =============================================================================
# C compilation
#
# This one pattern rule handles both:
#   kernel/*.c
#   arch/i686/*.c
#
# because the prerequisite path is preserved after build/i686/.
# =============================================================================

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# =============================================================================
# Assembly compilation
#
# Lowercase .s:
#   assembled directly without preprocessing
#
# Uppercase .S:
#   compiled through GCC so CPPFLAGS and #include work
# =============================================================================

$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# =============================================================================
# Kernel library
# =============================================================================

libk: $(LIBK)

$(LIBK):
	$(MAKE) -C $(LIBK_DIR) ARCH=$(ARCH)
	@mkdir -p $(dir $@)
	cp $(LIBK_DIR)/build/libk.a $@

# =============================================================================
# C library and sysroot
# =============================================================================

libc: $(SYSROOT_STAMP)

$(SYSROOT_STAMP): $(LIBC) $(CRT0) $(LIBC_HEADERS)
	mkdir -p $(SYSROOT)/usr/include
	mkdir -p $(SYSROOT)/usr/lib
	cp -Ru $(LIBC_INC)/* $(SYSROOT)/usr/include/
	cp -u $(LIBC) $(SYSROOT)/usr/lib/
	cp -u $(CRT0) $(SYSROOT)/usr/lib/
	touch $@

$(LIBC) $(CRT0):
	$(MAKE) -C $(LIBC_DIR) ARCH=$(ARCH)

# =============================================================================
# Graphics library
# =============================================================================

libgfx: $(LIBGFX)

$(LIBGFX):
	$(MAKE) -C $(LIBGFX_DIR) install ARCH=$(ARCH)

# =============================================================================
# User programs and initrd
# =============================================================================

user: libc
	$(MAKE) -C $(USER_DIR)

$(FS): user
	python3 initrd.py

img: $(FS)

# =============================================================================
# ISO
# =============================================================================

iso: kernel libc user $(FS)
	rm -rf $(ISO_DIR)
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/$(KERNEL_BIN)
	cp grub.cfg $(ISO_DIR)/boot/grub
	cp $(FS) $(ISO_DIR)/boot/
	grub-mkrescue -o $(ISO) $(ISO_DIR)

# =============================================================================
# QEMU
# =============================================================================

run:
	qemu-system-i386 \
		-cdrom $(ISO) \
		-serial stdio \
		-monitor none

# =============================================================================
# Cleanup
# =============================================================================

clean:
	$(MAKE) -C $(LIBC_DIR) clean
	$(MAKE) -C $(LIBK_DIR) clean
	$(MAKE) -C $(LIBGFX_DIR) clean
	$(MAKE) -C $(USER_DIR) clean

	rm -rf $(BUILD_ROOT)
	rm -rf $(SYSROOT)
	rm -rf $(ISO_DIR)

	rm -f $(KERNEL_BIN)
	rm -f $(FS)
	rm -f $(ISO)
