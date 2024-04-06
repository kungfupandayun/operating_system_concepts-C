#include "process.h"

/**
  creating and initialiazing queue head
*/

LIST_HEAD(queue);

typedef struct __value_hash {
  void* adr_phys;
  void* adr_virt;
  int nb_proc;
} _val;

// its value is the pid of the active proc
uint32_t active = 0;
uint32_t nb_shared_page = 0;
// keeps track of the number of procs created
// will be the pid of the next proc to be created
extern int32_t sys_time(void);


static int32_t getNewCase() {
  int i = 0;
  for (i=0; i < NBPROC && (proc_table[i] != NULL
                                        && proc_table[i]->state != DEAD); i++);
  return i;
}

static void copier_code(void* start, void* end, unsigned* pgdir) {
  int l = ((int)end - (int)start)/4096;
  int t = ((int)end - (int)start) - l*4096;

  char* debut = (char*) start;
  unsigned adr_virtuelle = 0x40000000u; // 1Go
  char* bloc;

  for (int k = 0; k < l; k++) {

    bloc = (char*) page_alloc();
    map_page(pgdir, adr_virtuelle, (unsigned) bloc, PAGE_TABLE_USR_RW);

    for (int j=0; j<4096; j++) {
      bloc[j] = debut[j];
    }

    adr_virtuelle += 4096;
    debut += 4096;
  }

  bloc = (char*) page_alloc();
  map_page(pgdir, adr_virtuelle, (unsigned) bloc, PAGE_TABLE_USR_RW);

  for (int m=0; m < t; m++) {
    bloc[m] = debut[m];
  }

}

static int allouer_pile(int ssize, unsigned* pgdir, void* arg) {
  int l = ssize/4096;
  int t = ssize - l*4096;

  unsigned adr_virtuelle = 0x80000000u; // 2 Go
  unsigned* bloc;

  for (int i=0; i<l; i++) {
    bloc = page_alloc();
    map_page(pgdir, adr_virtuelle, (unsigned) bloc, PAGE_TABLE_USR_RW);
    adr_virtuelle += 4096;
  }
  if (t > 0) {
    bloc = page_alloc();
    map_page(pgdir, adr_virtuelle, (unsigned) bloc, PAGE_TABLE_USR_RW);
    adr_virtuelle += 4096;

  }
  // On place au sommet de la pile les arguments
  int* pile = (int*) bloc;
  pile[1023] = (int) arg;
  pile[1022] = FINISH_USER;
  pile[1019] = (int) arg;

  return adr_virtuelle-8; // correspond à la case de la fonction d'exit
}

static void exit_code(unsigned* pgdir) {
  char* bloc = (char*) page_alloc();
  map_page(pgdir, FINISH_USER, (unsigned) bloc, PAGE_TABLE_USR_RW);
  bloc[0] = 0x89;
  bloc[1] = 0xc3;
  bloc[2] = 0xb8;
  bloc[3] = 0x06;
  bloc[7] = 0xcd;
  bloc[8] = 0x31;
  bloc[9] = 0xc3;
}


int32_t getpid(void){
  return active;
}



int kill(int pid){
 //On verifie que le pid est valide
 if (pid == 0 || proc_table[pid] == NULL || proc_table[pid]->state == DEAD
                                          || proc_table[pid]->state == ZOMBIE) {
   return -1;
 }
 else {
   int daddyPid=proc_table[pid]->daddy_pid;
   // On regarde si le processus a un père
   if (daddyPid == -1) {
     proc_table[pid]->state=DEAD;
   }
   else {
     //On verifie que le père exite
     if (proc_table[daddyPid]==NULL) {
       proc_table[pid]->state=DEAD;
     }

     int dadState = proc_table[daddyPid]->state;
     //Et en plus, on verifie que le père n'est ni mort ni zombie
     if(dadState==DEAD || dadState == ZOMBIE) {
       proc_table[pid]->state=DEAD;
     }
     else {
       proc_table[pid]->state=ZOMBIE;
     }

     //Parcours de la liste des fils
     for (int childPid = proc_table[pid]->child_pid; childPid != -1;
                                childPid = proc_table[childPid]->brother_pid) {
        proc_t* curr_proc = proc_table[childPid];

        if (curr_proc->state == ZOMBIE) {
          curr_proc->state = DEAD;
        }
        else {
          curr_proc->daddy_pid = -1;
        }
    }
    if (daddyPid != -1 && proc_table[daddyPid]->state == CHILD_BLOCK) {
      proc_table[daddyPid]->state = READY;
    }
   }
   proc_table[pid]->exit_status=0;
   if (getpid() != pid) {
     queue_del(proc_table[pid], list);
   } else {
     scheduler();
   }
   return 0;
 }


}


 void exit(int retval){
   int pid = getpid();

   int daddyPid=proc_table[pid]->daddy_pid;
   // On regarde si le processus a un père
   if (daddyPid == -1) {
     proc_table[pid]->state=DEAD;
   }
   else {
     //On verifie que le père exite
     if (proc_table[daddyPid]==NULL) {
       proc_table[pid]->state=DEAD;
     }

     int dadState = proc_table[daddyPid]->state;
     //Et en plus, on verifie que le père n'est ni mort ni zombie
     if(dadState==DEAD || dadState == ZOMBIE) {
       proc_table[pid]->state=DEAD;
     }
     else {
       proc_table[pid]->state=ZOMBIE;
     }

     //Parcours de la liste des fils
     for (int childPid = proc_table[pid]->child_pid; childPid != -1;
                                childPid = proc_table[childPid]->brother_pid) {
        proc_t* curr_proc = proc_table[childPid];

        if (curr_proc->state == ZOMBIE) {
          curr_proc->state = DEAD;
        }
        else {
          curr_proc->daddy_pid = -1;
        }
    }

  proc_table[getpid()]->exit_status=retval;
  }
  if (daddyPid != -1 && proc_table[daddyPid]->state == CHILD_BLOCK) {
    proc_table[daddyPid]->state = READY;
  }
  scheduler();
  while(1);
}

void supprime_fils(int activePID, int pid){
   if (pid == proc_table[activePID]->child_pid) {
        proc_table[activePID]->child_pid = proc_table[pid]->brother_pid;
      }
      else {
        // On cherche le fils precedant le fils attendu dans la liste des fils
        // le précédant au début est le fils du pere
        int previous_child_pid = proc_table[activePID]->child_pid;
        // On parcourt la liste des fils jusqu'à atteindre le fils attendu et on
        // stocke le fils le précédant
        // childPid vaut le pid du frere du fils du pere
        for (int childPid = proc_table[previous_child_pid]->brother_pid;
                              childPid != -1 && childPid != pid;
                                childPid = proc_table[childPid]->brother_pid) {
          previous_child_pid = proc_table[previous_child_pid]->brother_pid;
        }
        // On supprime le fils attendu
        proc_table[previous_child_pid]->brother_pid =
                                                  proc_table[pid]->brother_pid;
      }
}

int waitpid(int pid, int *retvalp){
  int activePID=getpid();
  if (pid >= 0) {
    // On verifie que le processus qu'on doit attendre existe et que son pere
    // est bien le processus appelant
    if (proc_table[pid] != NULL && proc_table[pid]->state != DEAD
                                  && proc_table[pid]->daddy_pid == activePID) {
      while (proc_table[pid]->state != ZOMBIE) {
        proc_table[activePID]->state = CHILD_BLOCK;
        queue_add(proc_table[activePID],&queue,proc_t,list,priority);
        // On passe la main à un autre processus pour attendre
        scheduler();
      }

      if (retvalp != NULL) {
        *retvalp = proc_table[pid]->exit_status;
      }

      // On met à mort le processus attendu
      proc_table[pid]->state = DEAD;
      //On enlève le processus attendu de la liste des fils
      supprime_fils( activePID,  pid);
      return pid;
    }
    return -1;
  }
  else {
    // On verifie que le pere a des enfants
    if (proc_table[activePID]->child_pid != -1) {
      int pid_child_dead = -1;
      // On boucle tant qu'on ne trouve pas un fils mort ou zombie
      while (pid_child_dead == -1) {
        // On parcourt donc la liste des fils pour en trouver un
        for (int childPid = proc_table[activePID]->child_pid; childPid != -1;
                                childPid = proc_table[childPid]->brother_pid) {
          int state = proc_table[childPid]->state;
          if (state == ZOMBIE) {
            pid_child_dead = childPid;
            break;
          }
        }

      // On ordonnance le processus si il n'a pas trouvé de fils mort ou zombie
        if (pid_child_dead == -1) {
          proc_table[activePID]->state = CHILD_BLOCK;
          queue_add(proc_table[activePID],&queue,proc_t,list,priority);
          // On passe la main à un autre processus pour attendre
          scheduler();
        }

      }

      if (retvalp != NULL) {
        *retvalp = proc_table[pid_child_dead]->exit_status;
      }
      // On met à mort le processus attendu
      proc_table[pid_child_dead]->state = DEAD;
      //On enlève le processus attendu de la liste des fils
      supprime_fils( activePID,  pid_child_dead);
      return pid_child_dead;
    }
    return -1;
  }
 }

int getprio(int pid){
  if (pid > 0 && pid < NBPROC && proc_table[pid] != NULL
        && proc_table[pid]->state != DEAD && proc_table[pid]->state != ZOMBIE) {
    return proc_table[pid]->priority;
  }
  return -1;
}

int chprio(int pid, int newprio){
  if (pid > 0 && pid < NBPROC && newprio >= 1 && newprio <= MAXPRIO
                  && proc_table[pid] != NULL && proc_table[pid]->state != DEAD
                                          && proc_table[pid]->state != ZOMBIE) {
    int old_prio = proc_table[pid]->priority;
    proc_table[pid]->priority = newprio;
    // suppression et rajout pour que le proc avec la nouvelle prio soit le + jeune
    if (pid != getpid()) {
      queue_del(proc_table[pid], list);
      queue_add(proc_table[pid], &queue, proc_t, list, priority);
    }

    // On verifie si le processus actif l'est toujours suite au changement de
    // priorite
    proc_t *temp_top = queue_top(&queue, proc_t, list);
    if (temp_top!=NULL && proc_table[getpid()]->priority < temp_top->priority)
      scheduler();
    return old_prio;
  }
  return -1;
}

char *getname(void){
  return proc_table[active]->name;
}

void scheduler(){
  int previous_pid = getpid();
  // les processus dans l'etat mort, zombie ou bloques par n'importe quoi
  // rendent la main à un autre processus
  if(proc_table[previous_pid]->state != DEAD
                  && proc_table[previous_pid]->state != ZOMBIE
                      && proc_table[previous_pid]->state != CHILD_BLOCK) {
    proc_t *temp_top = queue_top(&queue, proc_t, list);
    // si l'ancien processus est actif, et la file n'est pas vide et qu'il n'y
    // a pas de processus de plus grandes priorites ou de priorites egales, il
    // reprend la main sinon on le remet dans la file d'attente
    if (proc_table[previous_pid]->state == ACTIVE && (temp_top==NULL
                    || proc_table[previous_pid]->priority > temp_top->priority))
      return;


    queue_add(proc_table[previous_pid],&queue,proc_t,list,priority);

    if (proc_table[previous_pid]->state==ACTIVE) {
      proc_table[previous_pid]->state = READY;
    }
  }

  // On parcourt la file des processus jusqu'à trouver un processus prêt ou
  // endormi dont la date de reveil est passe
  proc_t * ptr_parcours;
  proc_t *new_active = NULL;
  queue_for_each_prev(ptr_parcours,&queue,proc_t,list){
    if (ptr_parcours->state == READY || (ptr_parcours->state == ASLEEP
                            && ptr_parcours->wake_up_time <= current_clock())) {
      new_active = ptr_parcours;
      queue_del(ptr_parcours, list);
      break;
    }
  }


  active = new_active->pid;
  new_active->state = ACTIVE;
  change_cr3(new_active->page_directory);
  ctx_sw(proc_table[previous_pid]->registers , new_active->registers);
}

// Cette fonction sert à lancer des processus qui ne sont pas des applications
// user. Elle ressemble beaucoup à la fonction start du début.
int start2(int pt_func, unsigned long ssize,
               int prio, const char *name, void *arg){

  //cherche d'un processus libre
  int32_t pid = getNewCase();
  if (pid == NBPROC) {
    printf("La table de processus est pleine\n");
    return -1;
  }
  if (prio < 1 || prio > MAXPRIO) {
    return -1;
  }

  if ( ssize > 0x800000) {
    return -1;
  }

  if (proc_table[pid] == NULL) {
    proc_table[pid] = (proc_t *)mem_alloc(sizeof(proc_t));
  } else {
    libere_pages(proc_table[pid]->page_directory);
  }

  //daddy est le celui qui crée
  proc_table[pid]->pid = pid;
  //si pid=0 , met en active, sinon ready
  proc_table[pid]->state = pid == 0 ? ACTIVE : READY;
  strncpy(proc_table[pid]->name, name, (unsigned long) NAME_SIZE);

  // Creation de la table des pages
  proc_table[pid]->page_directory = create_pgdir();

  proc_table[pid]->stack[STACK_SIZE - 3] = pt_func;
  proc_table[pid]->stack[STACK_SIZE - 2] = (int) finish_handler;
  proc_table[pid]->stack[STACK_SIZE - 1] = (int) arg ;

  proc_table[pid]->registers[ESP] = (int32_t) (proc_table[pid]->stack + STACK_SIZE - 3);
  proc_table[pid]->priority = prio;
  proc_table[pid]->ssize = ssize;

  INIT_LINK(&(proc_table[pid]->list));

  if (pid != 0) {
    queue_add(proc_table[pid],&queue,proc_t,list,priority);

    int daddy_pid = getpid();
    if (daddy_pid==0) {
      // Le processus de pid 0 est le processus principal, il ne peut pas avoir
      // un pere
      proc_table[pid]->daddy_pid = -1;
    }
    else {
      proc_table[pid]->daddy_pid = getpid();
    }

    proc_table[pid]->brother_pid = proc_table[getpid()]->child_pid;
    proc_table[getpid()]->child_pid = pid;
  }
  else {
    proc_table[pid]->daddy_pid = -1;
    proc_table[pid]->brother_pid = -1;
  }
  proc_table[pid]->child_pid = -1;

  if (prio > proc_table[getpid()]->priority) {
    scheduler();
  }
  return pid;
}

int start(const char *name, unsigned long ssize, int prio, void *arg) {
  //cherche d'un processus libre
  int32_t pid = getNewCase();
  if (pid == NBPROC) {
    printf("La table de processus est pleine\n");
    return -1;
  }
  if (prio < 1 || prio > MAXPRIO) {
    return -1;
  }

  if ( ssize > 0x800000) {
    return -1;
  }

  if (proc_table[pid] == NULL) {
    proc_table[pid] = (proc_t *)mem_alloc(sizeof(proc_t));
  }

  //daddy est le celui qui crée
  proc_table[pid]->pid = pid;
  //si pid=0 , met en active, sinon ready
  proc_table[pid]->state = pid == 0 ? ACTIVE : READY;
  strncpy(proc_table[pid]->name, name, (unsigned long) NAME_SIZE);

  // Creation de la table des pages
  proc_table[pid]->page_directory = create_pgdir();

  //Recuperer le code du processus
  struct uapps* desc = (struct uapps*) hash_get(tab_sym, (void*) name, NULL);
  copier_code(desc->start, desc->end, proc_table[pid]->page_directory);

  // Allouer et mapper la pile user
  int user_stack = allouer_pile(ssize+80, proc_table[pid]->page_directory, arg);

  // Code de la fonction finish d'un processus
  exit_code(proc_table[pid]->page_directory);

  // Pile kernel
  proc_table[pid]->stack[STACK_SIZE - 1] = 0x4b; // SS
  proc_table[pid]->stack[STACK_SIZE - 2] = user_stack; // Pile user
  proc_table[pid]->stack[STACK_SIZE - 3] = 0x202; // EFLAGS
  proc_table[pid]->stack[STACK_SIZE - 4] = 0x43; // CS
  proc_table[pid]->stack[STACK_SIZE - 5] = 0x40000000; // 1 Go (code)
  proc_table[pid]->stack[STACK_SIZE - 6] = (int) demare_proc; // Passage de kernel vers user

  proc_table[pid]->registers[ESP] = (int32_t) (proc_table[pid]->stack + STACK_SIZE - 6);
  proc_table[pid]->priority = prio;
  proc_table[pid]->ssize = ssize;

  INIT_LINK(&(proc_table[pid]->list));

  proc_table[pid]->send_wait_on = -1;
  proc_table[pid]->rcv_wait_on = -1;

  if (pid != 0) {
    queue_add(proc_table[pid],&queue,proc_t,list,priority);

    int daddy_pid = getpid();
    if (daddy_pid==0) {
      // Le processus de pid 0 est le processus principal, il ne peut pas etre
      // un pere
      proc_table[pid]->daddy_pid = -1;
    }
    else {
      proc_table[pid]->daddy_pid = getpid();
    }

    proc_table[pid]->brother_pid = proc_table[getpid()]->child_pid;
    proc_table[getpid()]->child_pid = pid;
  }
  else {
    proc_table[pid]->daddy_pid = -1;
    proc_table[pid]->brother_pid = -1;
  }
  proc_table[pid]->child_pid = -1;

  if (prio > proc_table[getpid()]->priority) {
    scheduler();
  }
  return pid;
}


void *shm_create(const char *key) {
  if(map==NULL){
    map=(hash_t*) mem_alloc(sizeof(hash_t));
    hash_init_string(map);
  }
  if (key == NULL) {
    return NULL;
  }
  void * cle = (void*) page_alloc();
  strncpy(cle, key, (unsigned long) NAME_SIZE);
  void *page = (void*) page_alloc();//allocation de la page de 4Ko
  if (page == NULL) {
    return NULL;
  }
  _val* value = (_val*) mem_alloc(sizeof(_val));
  value->adr_phys = page;
  value->adr_virt = (void*) (0xc0000000+nb_shared_page*0x1000);
  value->nb_proc = 1;
  nb_shared_page++;
  map_page(proc_table[getpid()]->page_directory, (unsigned) value->adr_virt,
                                (unsigned) value->adr_phys, PAGE_TABLE_USR_RW);
  int insere = hash_set(map, cle, value);
  if (insere != 0) {
    return NULL;
  }
  return value->adr_virt;
}

void *shm_acquire(const char *key) {

  _val* value = (_val*) hash_get(map, (void*)key, NULL);
  if (value != NULL) {
    value->nb_proc++;
    map_page(proc_table[getpid()]->page_directory, (unsigned) value->adr_virt,
                                (unsigned) value->adr_phys, PAGE_TABLE_USR_RW);
    return value->adr_virt;
  }
  return NULL;

}

void shm_release(const char *key) {
  _val* value = (_val*) hash_get(map, (void*)key, NULL);
  if (value != NULL) {
    unsigned * page_dir = proc_table[getpid()]->page_directory;
    unsigned adr_virtuelle = (unsigned) value->adr_virt;
    if (get_physaddr(adr_virtuelle, page_dir) != NULL) {
      value->nb_proc--;
      unmap_page(page_dir, adr_virtuelle);
      // Pour obliger le TLB à se mettre à jour, on recharge le page directory
      change_cr3(page_dir);
    }
    if (value->nb_proc == 0) {
      hash_del(map, (void*)key);
      page_free((unsigned*) value->adr_phys);
      mem_free(value, sizeof(_val));
    }
  }
}

void init_tab_sym() {
  int i = 0;
  tab_sym = (hash_t*) mem_alloc(sizeof(hash_t));
  hash_init_string(tab_sym);
  while (symbols_table[i].name != NULL) {
    //printf("%s\t", symbols_table[i].name);
    hash_set(tab_sym, (void*) symbols_table[i].name, (void*)(symbols_table+i));
    i++;
  }
}


const char* getStateName(state_t state) {
  switch (state) {
    case ACTIVE: return "ACTIVE";
    case READY: return "READY";
    case MSG_BLOCK: return "MSG_BLOCK";
    case IO_BLOCK: return "IO_BLOCK";
    case CHILD_BLOCK: return "CHILD_BLOCK";
    case ASLEEP: return "ASLEEP";
    case ZOMBIE: return "ZOMBIE";
    case DEAD: return "DEAD";
  }
  return "";
}

void print_all_processes(void) {
  printf("%-10s%-20s%-20s\n", "PID", "NAME", "STATE");

  for (int i = 1; i < NBPROC; i++) {
    if (proc_table[i] != NULL) {
      printf("%-10d%-20s%-20s\n", proc_table[i]->pid, proc_table[i]->name,
      getStateName(proc_table[i]->state));
    }
  }
}
