#!/bin/bash

#janky build script (need to refactor into a make file)


nasm -f bin boot.asm -o bootloader                                  #assembles the bootloader
dd if=/dev/zero of=disk.img bs=512 count=2880                       #create floppy disk
dd conv=notrunc if=bootloader of=disk.img bs=512 count=1 seek=0     #write the bootloader to the first sector
qemu-system-i386 -machine q35 -fda disk.img -gdb tcp::26000 -S      #run the qemu session with gdb enabled
