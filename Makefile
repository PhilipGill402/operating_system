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

.PHONY: all kernel libc run iso clean dirs

all: kernel

dirs:
	mkdir -p $(BUILD_DIR)

# Build libc only when explicitly requested or when kernel is requested
libc:
	$(MAKE) -C $(LIBC_DIR)
	mkdir -p $(SYSROOT)/usr/include
	mkdir -p $(SYSROOT)/usr/lib
	cp -R $(LIBC_INC)/* $(SYSROOT)/usr/include/
	cp $(LIBC) $(SYSROOT)/usr/lib/
	cp $(CRT0) $(SYSROOT)/usr/lib/

libk:
	$(MAKE) -C $(LIBK_DIR)

# Build kernel only when explicitly requested
kernel: $(KERNEL_BIN)

user:
	$(MAKE) -C $(USER_DIR)

img: user
	python3 initrd.py	

$(KERNEL_BIN): linker.ld $(OBJECTS) $(LIBK)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBK) $(LIBS)
	grub-file --is-x86-multiboot $@

$(LIBK):
	$(MAKE) -C $(LIBK_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

# Only build ISO when explicitly requested
iso: kernel img libc user
	rm -rf $(ISO_DIR)	
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/$(KERNEL_BIN)
	cp grub.cfg $(ISO_DIR)/boot/grub
	cp $(FS) $(ISO_DIR)/boot
	grub-mkrescue -o $(ISO) $(ISO_DIR)

# Run only; assumes kernel was already built earlier
run:
	qemu-system-i386 -cdrom $(ISO) -serial stdio -monitor none

clean:
	$(MAKE) -C $(LIBC_DIR) clean
	rm -rf $(BUILD_DIR)
	rm initrd.img
	rm -rf sysroot
	rm $(KERNEL_BIN)
	rm $(ISO)

