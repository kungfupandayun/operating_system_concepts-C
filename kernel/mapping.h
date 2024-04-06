#ifndef MAPPING_H
# define MAPPING_H

#include "cpu.h"
#include "string.h"
#include "freelist.h"

#define PAGE_DIR_FLAGS     0x00000003u
#define PAGE_DIR_USR_FLAGS 0x00000007u

#define PAGE_TABLE_USR_RW  0x00000007u
#define PAGE_TABLE_RO      0x000000001u
#define PAGE_TABLE_RW      0x000000003u

void map_page(unsigned *pdir, unsigned adr_virtuelle, unsigned adr_physique,
                                                              unsigned flags);
void unmap_page(unsigned *pdir, unsigned adr_virtuelle);
unsigned* create_pgdir();
void * get_physaddr(unsigned virtualaddr, unsigned *pdir);
void libere_pages(unsigned* pgdir);

#endif
