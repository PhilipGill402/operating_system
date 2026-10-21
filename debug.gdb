file build/riscv64/kernel.elf
target remot :1234

define regs
    info registers
    printf "sepc = 0x%lx\n", $sepc
    printf "scause = 0x%lx\n", $scause
    printf "stval = 0x%lx\n", $stval
end
