/*
 * Ensimag - Projet système
 * Copyright (C) 2012 - Damien Dejean <dam.dejean@gmail.com>
 *
 * Stub for console_putbytes system call.
 */

#include "ecran.h"


void console_putbytes(const char *s, int len) {
    
    (void)s;
    (void)len;
    /** To be implemented */

    for (int i = 0; i < len; i++) {
        defilement();
        traite_car(*s);
        s++;
    }
}

