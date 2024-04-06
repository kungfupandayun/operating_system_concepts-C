#include "msgq.h"
#include "process.h"

int tmp_pid;

int pcreate(int count) {

    if (count <= 0 || count > MAXCOUNT) return -1;
    int fid = -2;
    for(int i = 0 ; i < NBQUEUE ; i++){
        if (msg_queues[i].max_count == 0){
            fid = i;
            break;
        }
    }
    if(fid != -2){
        msg_queues[fid].fid = fid;
        msg_queues[fid].max_count = count;
        msg_queues[fid].count = 0;
        msg_queues[fid].preset = 0;
        msg_queues[fid].head = NULL;
        msg_queues[fid].tail = NULL;
    }
    return fid;
}

int pdelete(int fid) {
    if(!VALID_FID(fid)) return -1;
    destroy_list(&msg_queues[fid]);
    proc_t * ptr ;
    queue_for_each(ptr, &queue, proc_t, list) {
        if (ptr->send_wait_on == fid + 1|| ptr->rcv_wait_on == fid + 1){
            ptr->state = READY;
            ptr->send_wait_on = ptr->rcv_wait_on = -1;
        }
    }
    msg_queues[fid].max_count = 0;
    scheduler();
    return 0;
}

int psend(int fid, int message) {
    //invalid fid
    if(!VALID_FID(fid)) return -1;

    //Calcule pcount
    int count;
    assert (pcount(fid,&count)==0);

    // preset était fait
    if (msg_queues[fid].preset==1) {
        msg_queues[fid].preset=0;
        msg_queues[fid].count =0;
    }


    //file est pleine , bloque à l'emision
   if (msg_queues[fid].count == msg_queues[fid].max_count) {
        proc_table[getpid()]->state = MSG_BLOCK;
        /* fid + 1 car ce champs initialisé à 0
         et pour eviter des problèmes éventuels avec
         le file de fid = 0, ce champs sera tjrs décalé de 1
         les tests pour le déblocage prennent ceci en compte*/
        proc_table[getpid()]->send_wait_on = fid + 1;
        scheduler();
        // REVIENT DE SCHEDULING
         if (msg_queues[fid].preset==1 ||  msg_queues[fid].max_count == 0
                                                ||msg_queues[fid].max_count==msg_queues[fid].count){
            return -1;
        }
        add_msg(&msg_queues[fid], message);
        change_cr3(proc_table[getpid()]->page_directory);
        ctx_sw(proc_table[tmp_pid]->registers , proc_table[getpid()]->registers);
    }

    //file vide + proc bloque à la reception
    else if (msg_queues[fid].count == 0 && count<0){
        add_msg(&msg_queues[fid], message);
        proc_t * ptr=NULL;
        queue_for_each_prev(ptr, &queue, proc_t, list) {
        if (ptr->rcv_wait_on == fid + 1 &&  ptr->state==MSG_BLOCK){
                ptr->state = READY ;
                ptr->rcv_wait_on= -1;
                break;
        }
        }
        tmp_pid= ptr->pid;
        ptr->state=READY;
        change_cr3(ptr->page_directory);
        ctx_sw(proc_table[getpid()]->registers , ptr->registers);
        scheduler();
    }

    //msg disponible dans la file de msg
    else{
        add_msg(&msg_queues[fid], message);
    }

    return 0;
}

int preset(int fid) {
    if(!VALID_FID(fid)) return -1;

    destroy_list(&msg_queues[fid]);
    proc_t * ptr ;
    queue_for_each(ptr, &queue, proc_t, list) {
        if (ptr->send_wait_on == fid + 1 || ptr->rcv_wait_on == fid + 1){
            ptr->state = READY;
            // most_prio_in_fifo = ptr;
            ptr->send_wait_on = ptr->rcv_wait_on = -1;
        }
    }
    msg_queues[fid].preset = 1;
    scheduler();
    return 0;
}


//Renvoie l'etat courant d'une file;
//*pcount< 0 => nb de proc bloqué à la reception
//*pcount> msg_queues[fid].max_count  => nb de proc bloqué à l'émission
//*pcount = msg_queues[fid].count  => nb de message dans la file de mssg
int pcount(int fid, int* count) {
    if(!VALID_FID(fid)) return *count = -1;
    if (count!=NULL) {
      int nbr_wait_empty = 0;
      proc_t * ptr1 ;
      queue_for_each(ptr1, &queue, proc_t, list){
        if (ptr1->rcv_wait_on == fid + 1){
          nbr_wait_empty--;
        }
      }
      int nbr_wait_full = 0;
      proc_t * ptr2 ;
      queue_for_each(ptr2, &queue, proc_t, list) {
        if (ptr2->send_wait_on == fid + 1){
          nbr_wait_full++;
        }
      }
      *count = nbr_wait_full + msg_queues[fid].count + nbr_wait_empty;
    }
    return 0;
}

int preceive(int fid,int *message) {
    //test invalid
    if(!VALID_FID(fid)) return -1;

    //recupere état de file
    int count;
    assert (pcount(fid,&count)==0);


    //file vide, bloque à la reception
    if(msg_queues[fid].count==0){
        proc_table[getpid()]->state = MSG_BLOCK;
        proc_table[getpid()]->rcv_wait_on = fid + 1;
        scheduler();
        // REVIENT DE SCHEDULING
            if (msg_queues[fid].preset==1 ||  msg_queues[fid].max_count == 0 ){
                return -1;
             }
        pop_msg(&msg_queues[fid],message);
        change_cr3(proc_table[getpid()]->page_directory);
        ctx_sw(proc_table[tmp_pid]->registers , proc_table[getpid()]->registers);
        return 0;
    }
    //file pleine + proc bloque à l'émision
    else if(count > msg_queues[fid].count && msg_queues[fid].count==msg_queues[fid].max_count){
        proc_table[getpid()]->rcv_wait_on = -1;
        pop_msg(&msg_queues[fid],message);
        proc_t * ptr=NULL;
        queue_for_each_prev(ptr, &queue, proc_t, list) {
            if (ptr->send_wait_on == fid + 1 &&  ptr->state==MSG_BLOCK){
                ptr->state = READY ;
                ptr->send_wait_on= -1;
                break;
            }
        }
        tmp_pid= ptr->pid;
        ptr->state=READY;
        change_cr3(ptr->page_directory);
        ctx_sw(proc_table[getpid()]->registers , ptr->registers);
        scheduler();
    }
    // msg dans file + pas proc bloque à l'émision
    else {
        pop_msg(&msg_queues[fid],message);
    }
    return 0;

}

void add_msg(msg_q_t* file, int message) {
   // printf("mssg in %d \n",message);
    if(!file) return;
    if(file->max_count == file->count ) return;
    if(file->head == 0) {
        file->head = file->tail = mem_alloc(sizeof(msglist_t));
        file->head->msg = message;
        file->count++;
        file->head->next = 0;
        return;
    }
    file->tail->next = mem_alloc(sizeof(msglist_t));
    file->tail->next->msg = message;
    file->tail->next->next = 0;
    file->tail = file->tail->next;
    file->count++;
}

void destroy_list(msg_q_t * file){
    if(!file) return;
    msglist_t *p = file->head;
    msglist_t * mem;
    while(p != 0){
        mem = p->next;
        mem_free(p,sizeof(*p));
        p = mem;
    }
    file->tail = file->head = NULL;
}

void pop_msg(msg_q_t* file, int* message) {
    //printf("mssg out %d \n",*message);
    assert(file!=NULL);

    if(message) *message = file->head->msg;

    if (file->head == file->tail){
        mem_free(file->head,sizeof(*(file->head)));
        file->head = file->tail = NULL;
        file->count =0;
        return;
    }

    msglist_t * mem = file->head->next;
    mem_free(file->head,sizeof(*(file->head)));
    file->head = mem;
    file->count--;
}

int file_vide (msg_q_t* file){
    return (file->head->msg=0);
}
