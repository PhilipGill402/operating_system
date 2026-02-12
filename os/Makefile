BUILD_DIR=../build/os
KERNEL_ELF=$(BUILD_DIR)/kernel.elf
KERNEL_BIN=$(BUILD_DIR)/kernel.bin
KERNEL_LD=../kernel.ld


CFLAGS+=-ffreestanding -fno-pie -fno-pic -fno-stack-protector -nostdlib -Wall -Wextra

OS_SRCS := $(wildcard *.c)
OS_OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(OS_SRCS))
ASM_SRCS := $(wildcard *.asm)
ASM_OBJS := $(patsubst %.asm, $(BUILD_DIR)/%.o, $(ASM_SRCS))

all: $(KERNEL_BIN)

$(BUILD_DIR)/%.o: %.c
	gcc $(CFLAGS) -m32 -c  $< -o $@

$(BUILD_DIR)/%.o: %.asm
	nasm -f elf32 $< -o $@

$(KERNEL_ELF): $(OS_OBJS) $(ASM_OBJS)
	ld -m elf_i386 -T $(KERNEL_LD) -o $@ $(OS_OBJS) $(ASM_OBJS)

$(KERNEL_BIN): $(KERNEL_ELF)
	objcopy -O binary $< $@

clean:
	rm $(BUILD_DIR)/*



