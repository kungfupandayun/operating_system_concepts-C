    .text
# cette directive sert a rendre l'etiquette publique
    .globl traitant_IT_32
# debut du traitant
traitant_IT_32:
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
# appel a la fonction C realisant le traitant
# a decommenter quand vous l'aurez implantee
   call tic_PIT
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
    add $20, %esp
    movl %esp, tss+4
    sub $20, %esp
# fin du traitant
    iret
