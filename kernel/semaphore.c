//Semaphore
#include "process.h"
#include "mem.h"
#define NBSEM 10  

typedef struct _semaphore {
    short int value;
}semaphore;

semaphore* sem_table[NBSEM];


int screate(short int count){
    int i=0;
    while(sem_table[i]!=NULL || i<NBSEM){
        i++;
    }
    if(i<NBSEM && count>0){
        sem_table[i] = (semaphore *)mem_alloc(sizeof(semaphore));
        sem_table[i]->value=count;
        return i;
    }
    else{
        return -1;
    } 
}

// int wait(int sem){

//     if(sem<0 || sem>NBSEM || (sem_table[sem]==NULL)) return -1;
//     //-2 en cas de dépassement
//     (sem_table[sem]->value)--;
//     if(sem_table[sem]->value<0){
//         proc_table[getpid()]->state = SEM_BLOCK;
//     }


// }

int scount(semaphore* sem){
    int res=0;
    //Sem val invalide
    if(sem->value<=0) return -1;
    else return res||(int)sem->value;
}

//int sdelete(int sem){}


// int sreset(sem, count);

// int signal(int sem);
// int signaln(int sem, short int count);

// int try wait(int sem);

