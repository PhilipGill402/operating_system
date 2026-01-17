BOOTLOADER=bootloader.o
OS=sample.o
DISK_IMG=disk.img

BOOTLOADER_SRCS := $(wildcard *.asm)
BOOTLOADER_OBJS := $(patsubst %.asm, %.0, $(BOOTLOADER_SRCS))



all: bootdisk

%.o: %.asm
	nasm -f bin $< -o $@

bootdisk: $(BOOTLOADER_OBJS)
	dd if=/dev/zero of=$(DISK_IMG) bs=512 count=2880
	dd conv=notrunc if=$(BOOTLOADER) of=$(DISK_IMG) bs=512 count=1 seek=0
	dd conv=notrunc if=$(OS) of=$(DISK_IMG) bs=512 count=1 seek=1

