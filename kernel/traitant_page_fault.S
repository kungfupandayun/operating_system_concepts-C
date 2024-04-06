.text
.globl traitant_IT_14

traitant_IT_14:
# sauvegarde des registres de segments
    push %ds
    push %es
    push %fs
    push %gs
# sauvegarde des registres importants
    pushl %eax
    pushl %edx
    pushl %ecx
# passage en mode superviseur / sauvegarde
    movl $0x18, %eax
    movl %eax,%ds
    movl %eax,%es
    movl %eax,%fs
    movl %eax,%gs
# On tue le processus actif
    call    getpid;
    pushl   %eax;
    call    kill;
# restauration des registres importants
    popl %ecx
    popl %edx
    popl %eax
# retour au mode utilisateur
    pop %gs
    pop %fs
    pop %es
    pop %ds
# sauvegarde de pile superviseur dans le tss
    movl %esp, tss+4
    iret;
