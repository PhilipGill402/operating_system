BUILD_DIR=../build/bootloader
BOOTLOADER_BIN=$(BUILD_DIR)/bootloader.bin
BOOT_LD=../bootloader.ld

BOOTLOADER_SRCS := $(wildcard *.asm)
BOOTLOADER_OBJS := $(patsubst %.asm, $(BUILD_DIR)/%.o, $(BOOTLOADER_SRCS))


all: $(BOOTLOADER_BIN)

$(BUILD_DIR)/%.o: %.asm
	nasm -f elf32 $< -g -o $@

$(BOOTLOADER_BIN): $(BOOTLOADER_OBJS)
	ld -m elf_i386 -T $(BOOT_LD) -o $@ $(BOOTLOADER_OBJS)

clean:
	rm $(BUILD_DIR)/*
