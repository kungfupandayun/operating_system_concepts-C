#include "cpu.h"
#include "stdint.h"
#include "ecran.h"
#include "string.h"
#include "stdbool.h"
#include <stdio.h>

uint16_t ligne = 1;
uint16_t colonne = 0;

bool firstChar = true;

uint16_t *ptr_mem(uint32_t lig, uint32_t col) {
    uint16_t *ptr = (uint16_t *)(0xB8000 + 2 * (lig * 80 + col));
    return ptr;
}

void ecrit_car(uint32_t lig, uint32_t col, char c, uint8_t format) {
    uint16_t *ptr = ptr_mem(lig, col);
    *ptr = format;
    *ptr = (*ptr) << 8 | c;
}

void place_curseur(uint32_t lig, uint32_t col) {
    uint16_t pos = col + lig * 80;

    colonne = col;
    ligne = lig;

    outb(0x0F, 0x3D4);
    outb(pos & 0xFF, 0x3D5);
    outb(0x0E, 0x3D4);
    outb(pos >> 8, 0x3D5);
}

void efface_ecran(void) {
    uint8_t format = 0b00001111;
    place_curseur(1,0);
    ligne = 1;
    colonne = 0;

    for (int i = 0; i < 80; i++) {
        for (int j = 0; j < 25; j++) {
            ecrit_car(j, i, 0, format);
        }
    }
}

// TO BE TESTED
void defilement(void) {
    if (ligne >= 24) {
        for (int i = 1; i < 25; i++) {
            memmove(ptr_mem(i,0), ptr_mem(i+1, 0), 160);
        }
        ligne -= 1;
    }
}

void traite_car(char c) {
    if (firstChar) {
        efface_ecran();
        firstChar = false;
    }
    
    switch(c) {
        case '\b':
             if (colonne != 0) //place_curseur(ligne, colonne - 1);
             colonne--;
            break;
        case '\t':
            //place_curseur(ligne, colonne + colonne%8);
            colonne=(((colonne)/8)+1)*8;
            break;
        case '\n':
            //place_curseur(ligne + 1, 0);
            ligne++;
            colonne=0;
            break;
        case '\f':
            efface_ecran();
            break;
        case '\r':
            //place_curseur(ligne, 0);
            colonne=0;
            break;
        default:
            if (c >= 32 && c <= 126) {
                ecrit_car(ligne, colonne, c, 15);
                colonne++;
                //place_curseur(ligne, colonne);
            }
            break;
    }
    if(colonne>=80) {
        colonne=0;
        ligne++;
    }
    if(ligne>=25){defilement();}
    place_curseur(ligne,colonne);
}

// Used to print the PIDs 
void print_top_left(char *s) {
    int length = 0;
    while (*(s + length) != 0) {
       length++;
    }
    for(int i=0;i<length;i++)
    { *(ptr_mem(0, 0))=s[i] + (0x0F<<8);}
}

// Used to print the clock
void print_top_right(char *s) {
    int length = 0;
    while (*(s + length) != 0) {
       length++;
    }
    int tmp_lig = ligne;
    int tmp_col = colonne;
    int i;
    for(i=0;i<length;i++)
    { *(ptr_mem(0, 70+i))=s[i] + (0x0F<<8);}
    place_curseur(tmp_lig, tmp_col);
}
