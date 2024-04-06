#include "mapping.h"

extern unsigned pgdir[];

/**
 * Create kernel initial memory mapping.
 */
static void map_kernel(unsigned* pagedir)
{
        unsigned i;


        for (i = 0; i < 64; i++) {
                pagedir[i] = pgdir[i];
        }
        for (i = 64; i < 1024; i++) {
                pagedir[i] = 0;
        }

}


void map_page(unsigned *pdir, unsigned adr_virtuelle, unsigned adr_physique,
                                                              unsigned flags)
{
                /* Page dir and table indexes */
                unsigned pd_index = adr_virtuelle >> 22;
                unsigned pt_index = (adr_virtuelle >> 12) & 0x3FFu;

                /* On vérifie que la table des pages existe sinon on l'alloue*/
                unsigned *ptable;
                if (pdir[pd_index] == 0) {
                  pdir[pd_index] = ((unsigned) page_alloc())|PAGE_DIR_USR_FLAGS;
                }
                /* Get Page tables */
                ptable = (unsigned*) (pdir[pd_index] & 0xFFFFF000);

                /* On vérifie que la page n'est pas déjà mappé */
                if (ptable[pt_index] != 0) {
                  assert(0);
                }
                ptable[pt_index] = (adr_physique & 0xFFFFF000) | flags;
}

void unmap_page(unsigned *pdir, unsigned adr_virtuelle) {
                /* Page dir and table indexes */
                unsigned pd_index = adr_virtuelle >> 22;
                unsigned pt_index = (adr_virtuelle >> 12) & 0x3FFu;

                unsigned *ptable = (unsigned*) (pdir[pd_index] & 0xFFFFF000);
                ptable[pt_index] = 0;
}

void * get_physaddr(unsigned virtualaddr, unsigned *pdir)
{
    unsigned pdindex = virtualaddr >> 22;
    unsigned ptindex = (virtualaddr >> 12) & 0x3FFu;

    if (pdir[pdindex] == 0) {
      return NULL;
    }

    unsigned * pt = (unsigned*) (pdir[pdindex] & 0xFFFFF000);
    if (pt[ptindex] == 0) {
      return NULL;
    }

    return (void *)((pt[ptindex] & 0xFFFFF000)+ (virtualaddr & 0xFFF));
}

//Allouer une structure de pagination pour un processeur
unsigned* create_pgdir()
{

        unsigned *pdir= (unsigned*) page_alloc();
        map_kernel(pdir);
        //return l'addresse à mettre dans cr3
        return pdir;
}

void libere_pages(unsigned* pdir) {
      unsigned* pgtable;

      for (int i=64; i< 1024; i++) {

        if (pdir[i] != 0) {

          pgtable = (unsigned*) (pdir[i] & 0xFFFFF000) ;

          // Libérer les pages de mémoire physique mappées
          for (int j=0; j < 1024; j++) {
            if (pgtable[j] != 0) {
              page_free((unsigned*) (pgtable[j] & 0xFFFFF000));
            }
          }
          // puis libérer la table des pages
          page_free(pgtable);
        }
      }
      // Libérer le page directory
      page_free(pdir);
}
