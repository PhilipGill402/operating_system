# Bare Bones style Makefile adapted to:
# - kernel/   -> source files (.c and .s)
# - include/  -> header files (.h)
# - build/    -> object files
# - linker.ld -> linker script in project root
#
# Intended workflow:
#   make        -> build kernel binary
#   make run    -> run kernel directly in QEMU
#   make iso    -> build bootable ISO inside Docker/container

ARCH := i686
TARGET := $(ARCH)-elf

CC := $(TARGET)-gcc
AS := $(TARGET)-as
LD := $(TARGET)-gcc

SRC_DIR := kernel
INC_DIR := include
BUILD_DIR := build
ISO_DIR := isodir

LIBC_DIR := libc
LIBK_DIR := libk
LIBK := $(BUILD_DIR)/libk.a
LIBC := $(LIBC_DIR)/build/libc.a
LIBC_INC := $(LIBC_DIR)/include
LIBK_INC := $(LIBK_DIR)/include
CRT0 := $(LIBC_DIR)/build/crt0.o
SYSROOT := sysroot

SYSROOT_LIBC := $(SYSROOT)/usr/lib/libc.a
SYSROOT_CRT0 := $(SYSROOT)/usr/lib/crt0.o
SYSROOT_STAMP := $(SYSROOT)/.libc-installed

KERNEL_BIN := kernel.bin
ISO := myos.iso
FS := initrd.img

CFLAGS := -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I$(INC_DIR) -I$(LIBK_INC) -g
ASFLAGS :=
LDFLAGS := -T linker.ld -ffreestanding -O2 -nostdlib
LIBS := -lgcc

C_SOURCES := $(shell find $(SRC_DIR) -type f -name '*.c')
ASM_SOURCES := $(shell find $(SRC_DIR) -type f -name '*.s')

C_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
ASM_OBJECTS := $(patsubst $(SRC_DIR)/%.s,$(BUILD_DIR)/%.o,$(ASM_SOURCES))
OBJECTS := $(ASM_OBJECTS) $(C_OBJECTS)

LIBC_HEADERS := $(shell find $(LIBC_DIR)/include -type f -name '*.h')

USER_DIR := bin

.PHONY: all run iso clean user libc libk

all: kernel

debug: CFLAGS += -DMALLOC_DEBUG
debug: kernel

kernel: $(KERNEL_BIN)

libc: $(SYSROOT_STAMP)

$(SYSROOT_STAMP): $(LIBC) $(CRT0) $(LIBC_HEADERS)
	$(MAKE) -C $(LIBC_DIR)
	mkdir -p $(SYSROOT)/usr/include
	mkdir -p $(SYSROOT)/usr/lib
	cp -Ru $(LIBC_INC)/* $(SYSROOT)/usr/include/
	cp -u $(LIBC) $(SYSROOT)/usr/lib/
	cp -u $(CRT0) $(SYSROOT)/usr/lib/
	touch $@

$(LIBC) $(CRT0):
	$(MAKE) -C $(LIBC_DIR)

libk: $(LIBK)

$(LIBK):
	$(MAKE) -C $(LIBK_DIR)

user: libc
	$(MAKE) -C $(USER_DIR)

$(FS): user
	python3 initrd.py

img: $(FS)

$(KERNEL_BIN): linker.ld $(OBJECTS) $(LIBK)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBK) $(LIBS)
	grub-file --is-x86-multiboot $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

iso: kernel libc user $(FS)
	rm -rf $(ISO_DIR)
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/$(KERNEL_BIN)
	cp grub.cfg $(ISO_DIR)/boot/grub
	cp $(FS) $(ISO_DIR)/boot
	grub-mkrescue -o $(ISO) $(ISO_DIR)

run:
	qemu-system-i386 -cdrom $(ISO) -serial stdio -monitor none

clean:
	$(MAKE) -C $(LIBC_DIR) clean
	$(MAKE) -C $(USER_DIR) clean
	rm -rf $(BUILD_DIR)
	rm -f $(FS)
	rm -rf $(SYSROOT)
	rm -f $(KERNEL_BIN)
	rm -f $(ISO)
	rm -rf $(ISO_DIR)
