    .text
    .globl change_cr3
# Structure de la pile en entree :
#   %esp + 4 : adresse du nouvel page directory
change_cr3:
    movl 4(%esp), %eax
    # changement de registre CR3
    movl %eax, %cr3
    # changement de la sauvegarde CR3 dans la TSS
    movl %eax, tss+28
    # on passe la main au nouveau processus
    ret
