/*
 * Ensimag - Projet système
 * Copyright (C) 2012 - Damien Dejean <dam.dejean@gmail.com>
 *
 * Stub for console_putbytes system call.
 */

extern void cons_write(const char *s, int len);

void console_putbytes(const char *s, int len) {

        (void)s;
        (void)len;
        /** To be implemented */
        cons_write(s,len);

}
