#include "debugger.h"
#include "cpu.h"
#include "horloge.h"
#include "process.h"
#include "ecran.h"
#include "msgq.h"
#include "freelist.h"
#include "cons_fonctions.h"


#define DUMMY_VAL 78

extern void init_syscalls();
extern void traitant_IT_14(void);

int idle(void* arg) {
	(void) arg;
	start2((int)&idle, STACK_SIZE*4, 3, "idle", NULL);
 //start2((int)&test_cons_read, STACK_SIZE*4, 128, "test_cons_read",NULL);
   start("test_shell",  STACK_SIZE*4, 128, (void*)5);
	sti();
	while (1) {
		hlt();
	}
	return 0;
}


void kernel_start(void)
{

	//test_ecran2();
	init_clavier_terminal();
	initialisation_horloge(); // A ne pas toucher : permet d'avoir la préemption
  	init_freelist();
  	init_tab_sym();
  	init_syscalls();
  	init_traitant_IT(14,traitant_IT_14);
	//tstA(NULL);
	idle(NULL);
	while(1)
	  hlt();

	return;
}
