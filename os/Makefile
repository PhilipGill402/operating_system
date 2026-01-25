BUILD_DIR=../build/os

CFLAGS+=-ffreestanding -fno-pie -fno-pic -fno-stack-protector -nostdlib -Wall -Wextra

OS_SRCS := $(wildcard *.c)
OS_OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(OS_SRCS))
ASM_SRCS := $(wildcard *.asm)
ASM_OBJS := $(patsubst %.asm, $(BUILD_DIR)/%.o, $(ASM_SRCS))

all: $(OS_OBJS) $(ASM_OBJS)

$(BUILD_DIR)/%.o: %.c
	gcc $(CFLAGS) -m32 -c  $< -o $@

$(BUILD_DIR)/%.o: %.asm
	nasm -f elf32 $< -o $@

clean:
	rm $(BUILD_DIR)/*



