#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "cpu.h"


#define BLOCKSIZE 4096

unsigned * freelist; /*  l'adresse of the first free block in memory */





/*separer les memoires libres en pages 4Ko)*/
void initPageList(unsigned * start, unsigned * end);
/*Always allocate a memory of size<4k*/
unsigned * page_alloc();
void page_free(  unsigned * block );
unsigned* init_freelist();
