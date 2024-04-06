#ifndef _PROCESS_H
#define _PROCESS_H

#include "cpu.h"
#include "stdint.h"
#include <stdio.h>
#include <string.h>
#include "horloge.h"
#include "../shared/queue.h"
#include "hash.h"
#include "mem.h"
#include "freelist.h"
#include "userspace_apps.h"
#include "mapping.h"


#define STACK_SIZE  1024
#define MAXPRIO     256
#define NBPROC      50
#define FINISH_USER 0xa0000000
#define NAME_SIZE   128
#define ESP         1
extern void ctx_sw(int *, int *);


typedef enum _PROCESS_STATE {
  ACTIVE,
  READY,
  MSG_BLOCK, // bloqué sur une file de messages
  IO_BLOCK,
  CHILD_BLOCK,
  ASLEEP,
  ZOMBIE,
  DEAD
} state_t;

typedef struct _process {
  int pid;
  char name[NAME_SIZE];
  state_t state;
  int registers[5];
  int stack[STACK_SIZE];
  unsigned long wake_up_time;
  int priority;
  link list;
  int daddy_pid;
  int child_pid;
  int brother_pid;
  int exit_status;
  int send_wait_on ; // fid de la file dont il est mis en attente en production
  int rcv_wait_on ;  // fid de la file dont il est mis en attente en consommation
  int ssize;
  unsigned* page_directory;
} proc_t;

extern void ctx_sw(int *, int *);
extern void finish_handler();
extern void change_cr3(unsigned*);
extern void demare_proc();
extern void console_putbytes(const char *s, int len);

int getpid(void);
int getprio(int pid);
int chprio(int pid, int newprio);
void exit(int retval);
int kill(int pid);
int waitpid(int pid, int *retvalp);
void scheduler();
int start2(int pt_func, unsigned long ssize, int prio,
                                                const char *name, void *arg);
int start(const char *name, unsigned long ssize, int prio, void *arg);
void *shm_create(const char *key);
void *shm_acquire(const char *key);
void shm_release(const char *key);
void init_tab_sym();

void print_all_processes(void);

proc_t* proc_table[NBPROC];
hash_t * map; //table de hachage (globale)
hash_t * tab_sym;

#endif //_PROCESS_H
