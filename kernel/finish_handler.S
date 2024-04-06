    .text
    .globl finish_handler

    #debut de finish_handler
    finish_handler:
      #on met la valeur de retour de la fonction en parametre de exit
      push %eax

      #sortie de finish_handler
      call exit
      
