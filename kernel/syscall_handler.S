.text
.globl traitant_IT_49

traitant_IT_49:
    #sauvegarde des registres de segments
    push %ds
    push %es
    push %fs
    push %gs

    #passage des paramètres
    push %ebp #dernier paramètre
    push %edi
    push %esi
    push %edx
    push %ecx
    push %ebx

    # passage en mode superviseur / sauvegarde
    movl $0x18, %ebx
    movl %ebx,%ds
    movl %ebx,%es
    movl %ebx,%fs
    movl %ebx,%gs

    # appel de l'appel système
    call *sys_call_table(,%eax,4)

    # restauration des registres
    pop %ebx
    pop %ecx
    pop %edx
    pop %esi
    pop %edi
    pop %ebp

    # retour au mode utilisateur
    pop %gs
    pop %fs
    pop %es
    pop %ds

    # sauvegarde de pile superviseur dans le tss
    add $20, %esp
    movl %esp, tss+4
    sub $20, %esp

    iret
