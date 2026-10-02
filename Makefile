# =============================================================================
# Target architecture
# =============================================================================

ARCH ?= i686

ARCH_DIR := arch/$(ARCH)
ARCH_INC_DIR := $(ARCH_DIR)/include
ARCH_LINKER_SCRIPT := $(ARCH_DIR)/linker.ld
ARCH_CONFIG := $(ARCH_DIR)/config.mk

# Allow architectures/toolchains to override this from config.mk if needed.
TARGET ?= $(ARCH)-elf

# =============================================================================
# Project directories
# =============================================================================

KERNEL_DIR := kernel
KERNEL_INC_DIR := include

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

KERNEL_ELF := $(BUILD_DIR)/kernel.elf
KERNEL_BIN := $(BUILD_DIR)/kernel.bin
ISO := $(BUILD_DIR)/myos.iso
FS := $(BUILD_DIR)/initrd.img

# =============================================================================
# Architecture configuration
#
# Each architecture supplies things such as:
#
#   QEMU_ARCH
#   QEMU_ARGS
#   RUN_DEPS
#   BOOT_MODE
#   POST_LINK_CHECK
#
# =============================================================================

ifeq ($(wildcard $(ARCH_CONFIG)),)
$(error Unsupported architecture "$(ARCH)": missing $(ARCH_CONFIG))
endif

include $(ARCH_CONFIG)

# =============================================================================
# Toolchain
#
# TARGET may be overridden by arch/$(ARCH)/config.mk.
# =============================================================================

CC := $(TARGET)-gcc
AS := $(TARGET)-as
LD := $(TARGET)-gcc
OBJCOPY := $(TARGET)-objcopy

# =============================================================================
# Compiler and linker flags
# =============================================================================

COMMON_CPPFLAGS := \
	-I$(KERNEL_INC_DIR) \
	-I$(LIBK_INC)

ARCH_CPPFLAGS := \
	-I$(ARCH_INC_DIR)

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

# Allow an architecture to append its own flags.
CFLAGS += $(ARCH_CFLAGS)
ASFLAGS += $(ARCH_ASFLAGS)
LDFLAGS += $(ARCH_LDFLAGS)
COMMON_CPPFLAGS += $(ARCH_CPPFLAGS_EXTRA)

# =============================================================================
# Source discovery
# =============================================================================

KERNEL_C_SOURCES := \
	$(shell find $(KERNEL_DIR) -type f -name '*.c')

KERNEL_ASM_SOURCES := \
	$(shell find $(KERNEL_DIR) -type f -name '*.s')

KERNEL_CPP_ASM_SOURCES := \
	$(shell find $(KERNEL_DIR) -type f -name '*.S')

ARCH_C_SOURCES := \
	$(shell find $(ARCH_DIR) -type f -name '*.c')

ARCH_ASM_SOURCES := \
	$(shell find $(ARCH_DIR) -type f -name '*.s')

ARCH_CPP_ASM_SOURCES := \
	$(shell find $(ARCH_DIR) -type f -name '*.S')

# =============================================================================
# Object paths
#
# kernel/kernel.c
#   -> build/<arch>/kernel/kernel.o
#
# arch/<arch>/boot/boot.S
#   -> build/<arch>/arch/<arch>/boot/boot.o
# =============================================================================

KERNEL_C_OBJECTS := \
	$(patsubst %.c,$(BUILD_DIR)/%.o,$(KERNEL_C_SOURCES))

KERNEL_ASM_OBJECTS := \
	$(patsubst %.s,$(BUILD_DIR)/%.o,$(KERNEL_ASM_SOURCES))

KERNEL_CPP_ASM_OBJECTS := \
	$(patsubst %.S,$(BUILD_DIR)/%.o,$(KERNEL_CPP_ASM_SOURCES))

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
	$(KERNEL_CPP_ASM_OBJECTS) \
	$(KERNEL_C_OBJECTS)

LIBC_HEADERS := \
	$(shell find $(LIBC_INC) -type f -name '*.h')

# =============================================================================
# Phony targets
# =============================================================================

.PHONY: \
	all \
	kernel \
	bin \
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

kernel: $(KERNEL_ELF)

# Explicit target for a raw flat kernel binary.
bin: $(KERNEL_BIN)

debug: COMMON_CPPFLAGS += -DMALLOC_DEBUG
debug: kernel

# =============================================================================
# Kernel link
# =============================================================================

$(KERNEL_ELF): $(ARCH_LINKER_SCRIPT) $(OBJECTS) $(LIBK)
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBK) $(LIBS)
	$(if $(POST_LINK_CHECK),$(POST_LINK_CHECK))

# Convert the linked ELF into a genuine raw binary.
$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@

# =============================================================================
# Generic kernel C compilation
#
# Generic kernel files receive only public include paths.
# They cannot include architecture-private headers.
# =============================================================================

$(BUILD_DIR)/kernel/%.o: kernel/%.c
	@mkdir -p $(dir $@)
	$(CC) \
		$(COMMON_CPPFLAGS) \
		$(CFLAGS) \
		-c $< \
		-o $@

# =============================================================================
# Architecture-specific C compilation
# =============================================================================

$(BUILD_DIR)/$(ARCH_DIR)/%.o: $(ARCH_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) \
		$(COMMON_CPPFLAGS) \
		$(ARCH_CPPFLAGS) \
		$(CFLAGS) \
		-c $< \
		-o $@

# =============================================================================
# Generic kernel assembly compilation
# =============================================================================

$(BUILD_DIR)/kernel/%.o: kernel/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/kernel/%.o: kernel/%.S
	@mkdir -p $(dir $@)
	$(CC) \
		$(COMMON_CPPFLAGS) \
		$(CFLAGS) \
		-c $< \
		-o $@

# =============================================================================
# Architecture-specific assembly compilation
# =============================================================================

$(BUILD_DIR)/$(ARCH_DIR)/%.o: $(ARCH_DIR)/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/$(ARCH_DIR)/%.o: $(ARCH_DIR)/%.S
	@mkdir -p $(dir $@)
	$(CC) \
		$(COMMON_CPPFLAGS) \
		$(ARCH_CPPFLAGS) \
		$(CFLAGS) \
		-c $< \
		-o $@

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
	$(MAKE) -C $(USER_DIR) ARCH=$(ARCH)

$(FS): user
	@mkdir -p $(dir $@)
	python3 initrd.py
	@if [ -f initrd.img ]; then mv initrd.img $@; fi

img: $(FS)

# =============================================================================
# ISO
#
# Only architectures whose config selects BOOT_MODE=grub-iso support this.
# =============================================================================

ifeq ($(BOOT_MODE),grub-iso)

ISO_KERNEL_NAME ?= kernel.elf

$(ISO): $(KERNEL_ELF) libc user $(FS)
	rm -rf $(ISO_DIR)
	mkdir -p $(ISO_DIR)/boot/grub

	cp $(KERNEL_ELF) $(ISO_DIR)/boot/$(ISO_KERNEL_NAME)
	cp grub.cfg $(ISO_DIR)/boot/grub/
	cp $(FS) $(ISO_DIR)/boot/

	i686-elf-grub-mkrescue -o $(ISO) $(ISO_DIR)

iso: $(ISO)

else

iso:
	@echo "ISO boot is not supported for architecture $(ARCH)"
	@false

endif

# =============================================================================
# QEMU
# =============================================================================

run: $(RUN_DEPS)
	qemu-system-$(QEMU_ARCH) $(QEMU_ARGS)

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
