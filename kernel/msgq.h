#ifndef __MSGQ_H
#define __MSGQ_H

#include "../shared/queue.h"
#include "stdint.h"
#include "process.h"
#include "mem.h"

#define VALID_FID(fid) ( (((fid < NBQUEUE) &&  (fid >= 0))                    \
                                  && msg_queues[fid].max_count!=0)  ? 1 : 0 )

extern struct list_link queue;
extern proc_t* proc_table[NBPROC];
#define NBQUEUE 10
#define MAXCOUNT 100
// liste chainée de messages
typedef struct _cell {
    int msg;
    struct _cell * next;
} msglist_t;

//file de message
typedef struct _msg_queue {
    int fid;
    int max_count;
    int count;
    int preset;
    msglist_t * head; // liste des messages
    msglist_t * tail; // last msg
} msg_q_t;

msg_q_t msg_queues[NBQUEUE];


int pcount(int fid, int *count);
int pcreate(int count);
int pdelete(int fid);
int preceive(int fid,int *message);
int preset(int fid);
int psend(int fid, int message);
void add_msg(msg_q_t* file, int message);
void pop_msg(msg_q_t* file, int* message);
void destroy_list(msg_q_t* file);
int file_vide (msg_q_t* file);



#endif //__MSGQ_H
