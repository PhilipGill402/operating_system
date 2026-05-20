.section .text
.global _start
.extern main
.extern exit

_start:
    # At entry, esp points to:
    #
    #   esp -> argc
    #          argv[0]
    #          argv[1]
    #          ...
    #          NULL
    #
    # We need to call:
    #
    #   main(argc, argv)

    movl (%esp), %eax        # eax = argc
    leal 4(%esp), %ebx       # ebx = argv

    pushl %ebx               # push argv
    pushl %eax               # push argc
    call main

    # main's return value is in eax.
    # Call exit(ret).
    pushl %eax
    call exit

.hang:
    jmp .hang
