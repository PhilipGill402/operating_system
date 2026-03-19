# Bare Bones style Makefile adapted to:
# - kernel/   -> source files (.c and .s)
# - build/    -> object files
# - linker.ld -> linker script in project root
#
# Intended workflow:
#   make        -> build kernel binary
#   make run    -> run kernel directly in QEMU (no grub-mkrescue)
#   make iso    -> build bootable ISO (inside Docker/container)

ARCH := i686
TARGET := $(ARCH)-elf

CC := $(TARGET)-gcc
AS := $(TARGET)-as
LD := $(TARGET)-gcc

CFLAGS := -std=gnu99 -ffreestanding -O2 -Wall -Wextra
ASFLAGS :=
LDFLAGS := -T linker.ld -ffreestanding -O2 -nostdlib
LIBS := -lgcc

KERNEL_BIN := myos.bin
ISO := myos.iso

SRC_DIR := kernel
BUILD_DIR := build
ISO_DIR := isodir

C_SOURCES := $(wildcard $(SRC_DIR)/*.c)
ASM_SOURCES := $(wildcard $(SRC_DIR)/*.s)

C_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
ASM_OBJECTS := $(patsubst $(SRC_DIR)/%.s,$(BUILD_DIR)/%.o,$(ASM_SOURCES))

OBJECTS := $(ASM_OBJECTS) $(C_OBJECTS)

.PHONY: all clean iso run dirs

all: $(KERNEL_BIN)

dirs:
	mkdir -p $(BUILD_DIR)

# Link kernel
$(KERNEL_BIN): linker.ld $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)
	grub-file --is-x86-multiboot $@

# Compile C sources into build/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | dirs
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble .s sources into build/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s | dirs
	$(AS) $(ASFLAGS) $< -o $@

# Build bootable ISO
# Only use this inside the Docker container where grub-mkrescue works
iso: $(KERNEL_BIN)
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/$(KERNEL_BIN)
	printf 'menuentry "myos" {\n\tmultiboot /boot/$(KERNEL_BIN)\n}\n' > $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) $(ISO_DIR)

# Run directly in QEMU without generating an ISO
run: $(KERNEL_BIN)
	qemu-system-i386 -kernel $(KERNEL_BIN)

clean:
	rm -rf $(BUILD_DIR) $(KERNEL_BIN) $(ISO) $(ISO_DIR)
