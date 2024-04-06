#include "cons_fonctions.h"
#include "msgq.h"


extern void traitant_IT_33(void);
extern void do_scancode(int sc);

// fonction appel par le traitant_IT_33
void keyboard_it(void){
  int sc;
  outb(0x20, 0x20);
  sc = inb(0x60);
  do_scancode(sc);
}



void keyboard_data(char *str){

    int pid=-1;
    //un pid!=-1 si il est en attente d'une entrée clavier
        proc_t * ptr_parcours;
        queue_for_each_prev(ptr_parcours,&queue,proc_t,list){
            if (ptr_parcours->state == IO_BLOCK) {
                pid=ptr_parcours->pid;
            break;
         }
        }

    //printf("END %lu END\n",pointEOL);
    assert(pointEOL<=TAMPON_SIZE);

    if(pointEOL==(TAMPON_SIZE)){
        if(str[0]==13){
           if(pid!=-1) proc_table[pid]->state=READY;
        }
        if(str[0]==127){
            if(pointEOL>0){
                pointEOL--;
            }
        }

    }
    else{
        if(str[0]==127){
            if(pointEOL>0){
                pointEOL--;
            }
        }else{
            tampon[pointEOL]=str[0];
            pointEOL++;
            if(str[0]==13){
               if(pid!=-1)  proc_table[pid]->state=READY;
            }
        }
        if(echo==1) traite_echo(str[0]);
    }
}

void traite_echo(char car){
    //Code ASCII
    if(car==9 || (car>=32 && car<=126))
        printf("%c",(char)car);
    //Enter
    else if(car==13)
        printf("%c",(char)10);
    //Ctrl+qqch
    else if(car<32){
        printf("^");
        printf("%c",(char)64+car);
    }
    //Backspace
    else if(car==127){
        printf("%c",(char)8);
        printf("%c",(char)32);
        printf("%c",(char)8);
    }
}

void kbd_leds(unsigned char leds) {
  outb(0xed, 0x60);
  outb(leds, 0x60);
}

/**
 echo=> (bool) autorise l'affichage direct sur l'écran en frappant
 point_EOL=> la taille du message dans le tampon
 findeLIgne=> (bool) indique si la longuer==pointer, si oui, devra mettre à nul à la prochain d'appel
*/
void init_clavier_terminal(){
    masque_IRQ(1, 0);
    init_traitant_IT(33, traitant_IT_33);
    pointEOL=0;
    echo=1;
    finDeLIgne=0;
}

char pop_tampon(){
    char ch=tampon[0];
    unsigned long i;
    for( i=0; i<pointEOL ;i++){
        tampon[i]=tampon[i+1];
    }
    tampon[i]=0;
    if(pointEOL>0){
        pointEOL--;
    }
    return ch;
}

unsigned long cons_read(char *string, unsigned long length){

    if(length==0) return 0;

    //le cas où il y a qu'une touche Entrée dans le tampon
    //mettre à nul à la prochain d'appel
    if(finDeLIgne==1) {
        string[0]=0;
        finDeLIgne=0;
        pop_tampon();
        // printf("END %lu END\n",pointEOL);
        return 0;
    }

    //le cas ou le tampon est vide, bloquer sur le processus
    if(tampon[0]==0){
        proc_table[getpid()]->state=IO_BLOCK;
        scheduler();
    }

   //Si le nb caractères est égale à la taille du string à remplir
    if((pointEOL-1)== length){
        finDeLIgne=1;
    }

    //boucle transfer les charactères dans le tampon au string
    unsigned long i;
    for( i=0; (i<=length && tampon[0]!=13 && tampon[0]!=0);i++)
    {
        string[i]=pop_tampon();
    }

    //le nb de caratères == length
    if(finDeLIgne==0 && tampon[0]==13) {
       pop_tampon();
    }

    //nb de caractères transférés
    return i;
}

int cons_write(const char *str, long size){
    console_putbytes(str,size);
    return 0;
}


void cons_echo(int on){
    echo=on;
}
