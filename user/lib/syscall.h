#ifndef SYSCALL_H
# define SYSCALL_H

extern void finish_handler_user();

extern int getpid(void);
extern int getprio(int pid);
extern int chprio(int pid, int newprio);
extern void exit(int retval);
extern int kill(int pid);
extern int waitpid(int pid, int *retvalp);
extern int start(const char *name, unsigned long ssize, int prio, void *arg);
extern void *shm_create(const char *key);
extern void *shm_acquire(const char *key);
extern void shm_release(const char *key);
extern void clock_settings(unsigned long *quartz, unsigned long *ticks);
extern unsigned long current_clock();
extern void wait_clock(unsigned long clock);
extern int pcount(int fid, int *count);
extern int pcreate(int count);
extern int pdelete(int fid);
extern int preceive(int fid,int *message);
extern int preset(int fid);
extern int psend(int fid, int message);
extern void cons_write(const char *s, int len);
extern void cons_read(const char *s, int len);
extern void cons_echo(int on);
extern void efface_ecran(void);
extern void print_all_processes(void);

#endif
