file build/riscv64/kernel.elf
target remote :1234
add-symbol-file fs/bin/test

define regs
    info registers
    printf "sepc = 0x%lx\n", $sepc
    printf "scause = 0x%lx\n", $scause
    printf "stval = 0x%lx\n", $stval
end
