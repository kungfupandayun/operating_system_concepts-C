#include <string.h>
#include "horloge.h"
#include "process.h"



unsigned long pointEOL;
int finDeLIgne;
int echo;

#define TAMPON_SIZE 20
char tampon[TAMPON_SIZE];

void keyboard_data(char *str);
void kbd_leds(unsigned char leds);
void traite_echo(char car);
unsigned long cons_read(char *string, unsigned long length);
void do_scancode(int scancode) ;
void init_clavier_terminal();
 extern void keyboard_it(void);
int cons_write(const char *str, long size);
void cons_echo(int on);