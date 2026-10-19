.section .text
.global _start
.extern main
.extern exit

_start:
    # At entry, sp points to:
    #
    #    sp -> argc
    #          argv[0]
    #          argv[1]
    #          ...
    #          NULL
    #
    # We need to call:
    #
    #   main(argc, argv)

    ld a0, 0(sp)    # a0 = argc 
    addi a1, sp, 8  # a1 = argv

    call main

    # main's return value is in a0.
    # Call exit(ret).
    call exit

.hang:
    j .hang
