#ifndef __ECRAN_H__
#define __ECRAN_H__

#include "stdint.h"
uint16_t *ptr_mem(uint32_t lig, uint32_t col);
void ecrit_car(uint32_t lig, uint32_t col, char c, uint8_t format);
void place_curseur(uint32_t lig, uint32_t col);
void defilement(void);
void efface_ecran(void);
void traite_car(char c);
void print_top_left(char *s);
void print_top_right(char *s);
#endif
