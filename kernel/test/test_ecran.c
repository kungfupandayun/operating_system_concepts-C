#include "stdio.h"
#include "debugger.h"
#include "cpu.h"
#include "test_ecran.h"

void test_ecran1(){
  
    printf("azertyuiopsdfghjklmuwxcvbn,;azertyuiopdfghjklm%%u*wxc123!:4567890vbn,;:ftttjftcgfxxfyjfjf,;:!,;:§/.?§/.\n");   

}

void test_ecran2(){

    printf("normalement N est le dernier caratere\n");
    printf("I am PO JIA YUN\b");
    printf("b\tb\tb\tb\tb\tb\tb\tb\tb\tje suis le meilleur!!!!\n");
    //si colonne=0 , recule \b n'a aucun effet.
    printf("azertyuiopsdfghjklmuwxazertyuiopsdfghjklazertyuiooqsdfghjklwxcvbn,dfghjklmddfghl\b");
    printf("Bonjour, vous etes tous des idiots");
    printf("\r");
}


void test_ecran3(){
  
    for (int i = 0; i < 1000; i++) {
        if (i%4 == 0) {
            printf("je suis le meilleur\n");
        } else if (i%4 == 1) {
            printf("Bart Simpson\n");
        } else if (i%4 == 2) {
            printf("Benedict Cumberbatch sait pas comment dire pingouin\n");
        } else {
            printf("Je suis tres beau et vous pas!!!! Ha Ha!! \n");
        }
    }  
    
}

void test_ecran4(){
for (int i = 0; i < 1000; i++) {
        printf("Bonjour");
    }  

}