BUILD_DIR=build
BOOT_OBJS=$(wildcard $(BUILD_DIR)/bootloader/*.o)
KERNEL_OBJS=$(wildcard $(BUILD_DIR)/os/*.o)
BOOTLOADER=$(BUILD_DIR)/bootloader/bootloader.o
KERNEL=$(BUILD_DIR)/os/kernel.o
LINKER=linker.ld
DISK_IMG=disk.img

all: bootdisk

.PHONY: bootloader os

bootloader:
	make -C bootloader

os:
	make -C os

bootdisk:  bootloader os
	ld -m elf_i386 -T $(LINKER) -o kernel.elf $(BOOT_OBJS) $(KERNEL_OBJS) 
	
	#dd if=/dev/zero of=$(DISK_IMG) bs=512 count=2880
	#dd conv=notrunc if=$(BOOTLOADER) of=$(DISK_IMG) bs=512 count=1 seek=0
	#dd conv=notrunc if=$(OS) of=$(DISK_IMG) bs=512 count=$$(($(shell stat --printf="%s" $(OS))/512)) seek=1

qemu-debug:
	qemu-system-i386 -machine q35 -fda $(DISK_IMG) -gdb tcp::26000 -S

qemu:
	qemu-system-x86_64 -fda kernel.elf

	#qemu-system-i386 -machine q35 -fda $(DISK_IMG)

clean:
	make -C bootloader clean
	make -C os clean
