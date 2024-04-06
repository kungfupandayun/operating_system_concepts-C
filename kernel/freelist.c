#include "freelist.h"
/**
 * Fonction initList couper la memoire en blocs de 4k
 * Chaque bloc contient l'adresse du bloc suivant
 * La memoire est alloué de haut en bas
 * Pointeur freelist pointe toujours sur la premier bloc vide dans la liste
 * Le dernier bloc contient vide
 */


void initPageList(unsigned * start, unsigned * end){
	   unsigned* ptr;
	   unsigned*  ptr_address;
     ptr_address= start + 1024;
     for (ptr = start; ptr < end-1024 ; ptr += 1024) {
			*ptr=(unsigned )ptr_address;
			ptr_address += 1024;
     }
     freelist=start;
}

 unsigned * page_alloc()
{
     if (freelist == NULL) {
          printf( "no space available" );
          return NULL;
     } else {
          unsigned * block;
          block = freelist;
          freelist = (unsigned *)*freelist;
					memset(block, 0, 4096);
          return block;
     }
}

void page_free(unsigned * block )
{
     assert((((unsigned)block) & 0xFFFFF000u) == ((unsigned)block));
	   if (block != NULL) {
       *block =(unsigned) freelist;
       freelist = block;
     }
}

unsigned* init_freelist(){
  initPageList((unsigned*)0x4000000,(unsigned*)0x10000000);
  return freelist;
}
