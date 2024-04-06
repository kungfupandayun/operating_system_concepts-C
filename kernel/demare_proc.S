    .text
    .globl demare_proc

demare_proc:
    movl $0x4b, %eax
    movl %eax, %ds
    movl %eax, %es
    movl %eax, %fs
    movl %eax, %gs
    # sauvegarde de pile superviseur dans le tss
    add $20, %esp
    movl %esp, tss+4
    sub $20, %esp
    iret
