
#include "process.h"
#include "msgq.h"
#include "horloge.h"
#include "cons_fonctions.h"
#include "ecran.h"

#define SYSCALL_MAX_NB 40

int sys_call_table[SYSCALL_MAX_NB]; // table des appels système

void init_syscall_table() {

    sys_call_table[0] = (int) start;
    sys_call_table[1] = (int) getpid;
    sys_call_table[2] = (int) getprio;
    sys_call_table[3] = (int) chprio;
    sys_call_table[4] = (int) kill;
    sys_call_table[5] = (int) waitpid;
    sys_call_table[6] = (int) exit;
    sys_call_table[7] = (int) cons_write;
    sys_call_table[8] = (int) cons_read;
     sys_call_table[9] = (int) cons_echo;
    // sys_call_table[10] = (int) scount;
    // sys_call_table[11] = (int)screate;
    // sys_call_table[12] = (int)sdelete;
    // sys_call_table[13] = (int)signal;
    // sys_call_table[14] = (int)signaln,
    // sys_call_table[15] = (int)sreset;
    // sys_call_table[16] = (int) try_wait;
    // sys_call_table[17] = (int) wait;
    sys_call_table[18] = (int) pcount;
    sys_call_table[19] = (int) pcreate;
    sys_call_table[20] = (int) pdelete;
    sys_call_table[21] = (int) preceive;
    sys_call_table[22] = (int) preset;
    sys_call_table[23] = (int) psend;
    sys_call_table[24] = (int) clock_settings;
    sys_call_table[25] = (int) current_clock;
    sys_call_table[26] = (int) wait_clock;
    // sys_call_table[27] = (int) sys_info;
    sys_call_table[28] = (int) shm_create;
    sys_call_table[29] = (int) shm_acquire;
    sys_call_table[30] = (int) shm_release;
    sys_call_table[31] = (int) efface_ecran; 
    sys_call_table[32] = (int) print_all_processes;
}
